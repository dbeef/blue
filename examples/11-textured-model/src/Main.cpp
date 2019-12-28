#include <blue/Context.hpp>
#include <blue/Timestep.hpp>
#include <blue/ShaderUtils.h>
#include <blue/ModelLoader.h>
#include <blue/camera/PerspectiveCamera.hpp>

#include <cmath>
#include <atomic>

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
		{ ShaderAttribute::Type::VEC3, ShaderAttribute::Purpose::COLOR, ShaderAttribute::Buffer::VERTEX},
		{ ShaderAttribute::Type::VEC2, ShaderAttribute::Purpose::TEXTURE_COORDINATE, ShaderAttribute::Buffer::VERTEX},
		{ ShaderAttribute::Type::VEC3, ShaderAttribute::Purpose::NORMAL, ShaderAttribute::Buffer::VERTEX}
	};

	// Simple model I created, that utilizes vertex painting.
	auto scene_ptr = models::load_scene("resources/SDKFZ.fbx");
	unsigned int vertex_counter = 0;

	// TODO: Supporting more textures; first merge image processing branch.
	// TODO: Not only vertices; list of paths to textures

	auto meshes = models::parse_scene(scene_ptr, attributes, vertex_counter);
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
		auto vertex_array = blue::Context::gpu_system().submit(CreateMeshEntity{ mesh, {}, attributes, vertex_counter }).get();
		vertex_arrays.push_back(vertex_array);
	}

	// Create environment

	auto environment = blue::Context::gpu_system().submit(CreateEnvironmentEntity{}).get();

	PerspectiveCamera camera(blue::Context::window().get_width(), blue::Context::window().get_height());

	blue::Context::gpu_system().submit(UpdateEnvironmentEntity_Projection{ environment, camera.get_projection() });
	blue::Context::gpu_system().submit(UpdateEnvironmentEntity_View{ environment, camera.get_view() });
	blue::Context::gpu_system().submit(SetClearColorEntity{ {0.5f, 0.5f, 0.5f} });

	// Submit render command consisting of compiled shader and uploaded mesh

	std::vector<RenderEntity> render_entities;

	for (auto& vertex_array : vertex_arrays)
	{
		RenderEntity entity;
		entity.position = { 0, 0, -2.5f };
		entity.shader = shader;
		entity.vertex_array = vertex_array;
		entity.scale = { 0.5f };
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
