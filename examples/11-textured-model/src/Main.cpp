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
	std::atomic_bool running {true};

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

	auto vertices = models::parse_scene(scene_ptr, attributes, vertex_counter);

	auto vertex_array = blue::Context::gpu_system().submit(CreateMeshEntity{ vertices, {}, attributes, vertex_counter}).get();

	// Create environment

	auto environment = blue::Context::gpu_system().submit(CreateEnvironmentEntity{}).get();

	PerspectiveCamera camera;

	blue::Context::gpu_system().submit(UpdateEnvironmentEntity_Projection{ environment, camera.get_projection() });
	blue::Context::gpu_system().submit(UpdateEnvironmentEntity_View{ environment, camera.get_view() });
	blue::Context::gpu_system().submit(SetClearColorEntity{ {0.5f, 0.5f, 0.5f} });

	// Submit render command consisting of compiled shader and uploaded mesh

	RenderEntity entity;
	entity.position = { 0, 0, -2.5f };
	entity.shader = shader;
	entity.vertex_array = vertex_array;
	entity.scale = 0.5f;
	entity.rotation = { 0, 0, 0, 0 };
	entity.environment = environment;

	entity.id = blue::Context::renderer().add(entity);

	// Start logics loop with timestep limited to 30 times per second:
	Timestep timestep(30);

	while (running)
	{
		timestep.mark_start();

		static glm::vec3 euler(0, 1.0f, 0);
		euler.y += 0.02f;
		entity.rotation = glm::quat(euler);
		blue::Context::renderer().update(entity);

		blue::Context::input().poll();
		timestep.mark_end();
		timestep.delay();
	}

	blue::Context::gpu_thread().stop();
	blue::Context::dispose();

	return 0;
}
