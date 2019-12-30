#include <blue/Context.hpp>
#include <blue/Timestep.hpp>
#include <blue/ShaderUtils.h>
#include <blue/ModelLoader.h>
#include <blue/camera/PerspectiveCamera.hpp>

#include <cmath>
#include <atomic>
#include <algorithm>

static const float CAMERA_SPEED = 0.5f;

int main(int argc, char* argv[])
{
	blue::Context::init();
	blue::Context::window().create(800, 600);

	// Register ESC key callback: 
	std::atomic_bool running{ true };

	blue::Context::input().registerKeyCallback({
		[&running]()
		{
			// FIXME: For some reason this fires up randomly.
			running = false;
		},
			SDLK_ESCAPE,
			SDL_KEYDOWN
		}
	);

	blue::Context::gpu_thread().run();

	// Now render thread is running and waiting for commands to process,
	// leaving this thread only for CPU logics. 

	// Issue the GPU thread with task of compiling shader program:

	auto compile_shader_entity = ShaderUtils::make_entity("resources/Triangle.vertex.glsl", "resources/Triangle.fragment.glsl");
	auto shader = blue::Context::gpu_system().submit(compile_shader_entity).get();

	// Issue the GPU thread with task of uploading mesh:

	Attributes attributes =
	{
		{ ShaderAttribute::Type::VEC3, ShaderAttribute::Purpose::VERTEX_POSITION, ShaderAttribute::Buffer::VERTEX},
		{ ShaderAttribute::Type::VEC2, ShaderAttribute::Purpose::TEXTURE_COORDINATE, ShaderAttribute::Buffer::VERTEX},
		{ ShaderAttribute::Type::VEC3, ShaderAttribute::Purpose::MATERIAL_AMBIENT, ShaderAttribute::Buffer::VERTEX},
		{ ShaderAttribute::Type::VEC3, ShaderAttribute::Purpose::MATERIAL_DIFFUSE, ShaderAttribute::Buffer::VERTEX},
		{ ShaderAttribute::Type::VEC3, ShaderAttribute::Purpose::MATERIAL_SPECULAR, ShaderAttribute::Buffer::VERTEX},
		{ ShaderAttribute::Type::FLOAT, ShaderAttribute::Purpose::MATERIAL_SHININESS, ShaderAttribute::Buffer::VERTEX},
		{ ShaderAttribute::Type::VEC3, ShaderAttribute::Purpose::NORMAL, ShaderAttribute::Buffer::VERTEX}
	};

	// Simple model I created, that utilizes vertex painting.
	//auto scene_ptr = models::load_scene("resources/low-poly-mill.fbx");
	auto scene_ptr = models::load_scene("resources/Low-Poly-Racing-Car.fbx");

	// TODO: Supporting more textures; first merge image processing branch.
	// TODO: Not only vertices; list of paths to textures

	std::vector<std::pair<Vertices, unsigned int>> meshes = models::parse_scene(scene_ptr, attributes);

	//meshes.erase(std::remove_if(meshes.begin(), meshes.end(), [](std::pair<Vertices, unsigned int>& v) {return v.first.empty();}));

	auto create_texture_render_entities = models::parse_textures(scene_ptr);
	std::vector<Texture> textures;
	std::vector<VertexArray> vertex_arrays;

	for (auto& entity : create_texture_render_entities)
	{
		auto texture_id = blue::Context::gpu_system().submit(entity).get();
		textures.push_back(texture_id);
	}

	for (auto& mesh : meshes)
	{
		auto vertex_array = blue::Context::gpu_system().submit(CreateMeshEntity{ mesh.first, {}, attributes, mesh.second }).get();
		vertex_arrays.push_back(vertex_array);
	}

	// Create environment

	auto environment = blue::Context::gpu_system().submit(CreateEnvironmentEntity{}).get();

	PerspectiveCamera camera(blue::Context::window().get_width(), blue::Context::window().get_height());

	glm::vec3 lightPos = { 30.0f, 33.0f, 30.f };
	glm::vec3 lightColor = { 1.0f, 1.0f, 1.0f };
	float ambientStrength = 1.0f;

	blue::Context::gpu_system().submit(UpdateEnvironmentEntity_Projection{ environment, camera.get_projection() });
	blue::Context::gpu_system().submit(UpdateEnvironmentEntity_View{ environment, camera.get_view() });
	blue::Context::gpu_system().submit(UpdateEnvironmentEntity_CameraPos{ environment, camera.get_position() });
	blue::Context::gpu_system().submit(UpdateEnvironmentEntity_LightPos{ environment, lightPos });
	blue::Context::gpu_system().submit(UpdateEnvironmentEntity_LightColor{ environment, lightColor });
	blue::Context::gpu_system().submit(UpdateEnvironmentEntity_AmbientStrength{ environment, ambientStrength });
	blue::Context::gpu_system().submit(SetClearColorEntity{ {0.5f, 0.5f, 0.5f} });

	// Register callbacks

	blue::Context::input().registerKeyCallback({
		[&running]() { running = false; },
		SDLK_ESCAPE,
		SDL_KEYDOWN
		}
	);

	auto w_callback = [&camera, &environment]()
	{
		camera.go_forward(CAMERA_SPEED);
		blue::Context::gpu_system().submit(UpdateEnvironmentEntity_View{ environment, camera.get_view() });
		blue::Context::gpu_system().submit(UpdateEnvironmentEntity_CameraPos{ environment, camera.get_position() });
	};
	blue::Context::input().registerKeyCallback({ w_callback, SDLK_w, SDL_KEYDOWN });

	auto s_callback = [&camera, &environment]()
	{
		camera.go_backward(CAMERA_SPEED);
		blue::Context::gpu_system().submit(UpdateEnvironmentEntity_View{ environment, camera.get_view() });
		blue::Context::gpu_system().submit(UpdateEnvironmentEntity_CameraPos{ environment, camera.get_position() });
	};
	blue::Context::input().registerKeyCallback({ s_callback, SDLK_s, SDL_KEYDOWN });

	auto a_callback = [&camera, &environment]()
	{
		camera.go_left(CAMERA_SPEED);
		blue::Context::gpu_system().submit(UpdateEnvironmentEntity_View{ environment, camera.get_view() });
		blue::Context::gpu_system().submit(UpdateEnvironmentEntity_CameraPos{ environment, camera.get_position() });
	};
	blue::Context::input().registerKeyCallback({ a_callback, SDLK_a, SDL_KEYDOWN });

	auto d_callback = [&camera, &environment]()
	{
		camera.go_right(CAMERA_SPEED);
		blue::Context::gpu_system().submit(UpdateEnvironmentEntity_View{ environment, camera.get_view() });
		blue::Context::gpu_system().submit(UpdateEnvironmentEntity_CameraPos{ environment, camera.get_position() });
	};
	blue::Context::input().registerKeyCallback({ d_callback, SDLK_d, SDL_KEYDOWN });

	auto mouse_move_callback = [&camera, &environment](double xpos, double ypos)
	{
		camera.mouse_rotation(xpos, ypos);
		blue::Context::gpu_system().submit(UpdateEnvironmentEntity_View{ environment, camera.get_view() });
		blue::Context::gpu_system().submit(UpdateEnvironmentEntity_CameraPos{ environment, camera.get_position() });
	};

	blue::Context::input().registerMouseMoveCallback(mouse_move_callback);

	// Submit render command consisting of compiled shader and uploaded mesh

	std::vector<RenderEntity> render_entities;

	for (auto& vertex_array : vertex_arrays)
	{
		RenderEntity entity;
		entity.position = { 0, 0, -2.5f };
		entity.shader = shader;
		entity.vertex_array = vertex_array;
		entity.scale = { 0.1f };
		entity.rotation = { 0, 0, 0, 0 };
		entity.environment = environment;

		for (std::size_t index = 0; index < textures.size(); index++)
		{
			if (index == BLUE_AVAILABLE_TEXTURE_SLOTS) break;
			entity.textures[index] = textures[index];
		}

		entity.id = blue::Context::renderer().add(entity);
		render_entities.push_back(entity);
	}

	// Start logics loop with timestep limited to 30 times per second:
	Timestep timestep(30);

	while (running)
	{
		timestep.mark_start();

		static glm::vec3 euler(0, 1.0f, 0);
		euler.y += 0.02f;

		for (auto& entity : render_entities)
		{
			entity.rotation = glm::quat(euler);
			blue::Context::renderer().update(entity);
		}

		blue::Context::input().poll();
		timestep.mark_end();
		timestep.delay();
	}

	blue::Context::gpu_thread().stop();
	blue::Context::dispose();

	return 0;
}
