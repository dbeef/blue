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
	auto shader_future = blue::Context::gpu_system().submit(compile_shader_entity);
	shader_future.wait();
	auto shader = shader_future.get();

	// Issue the GPU thread with task of uploading mesh:

	Attributes attributes =
	{
		{ ShaderAttribute::Type::VEC3, ShaderAttribute::Purpose::VERTEX_POSITION, ShaderAttribute::Buffer::VERTEX},
		{ ShaderAttribute::Type::VEC3, ShaderAttribute::Purpose::COLOR, ShaderAttribute::Buffer::VERTEX},
		{ ShaderAttribute::Type::VEC3, ShaderAttribute::Purpose::NORMAL, ShaderAttribute::Buffer::VERTEX},
	};

	// Simple model I created, that utilizes vertex painting.
	auto scene_ptr = models::load_scene("resources/PineTree.fbx");
	unsigned int vertex_counter = 0;
	auto vertices = models::parse_scene(scene_ptr, attributes, vertex_counter);
	auto vertex_array = blue::Context::gpu_system().submit(CreateMeshEntity{ vertices, {}, attributes, vertex_counter }).get();

	// Now create instanced VAO

	Attributes instanced_attributes =
	{
		{ ShaderAttribute::Type::VEC3, ShaderAttribute::Purpose::VERTEX_POSITION, ShaderAttribute::Buffer::VERTEX},
		{ ShaderAttribute::Type::VEC3, ShaderAttribute::Purpose::COLOR, ShaderAttribute::Buffer::VERTEX},
		{ ShaderAttribute::Type::VEC3, ShaderAttribute::Purpose::NORMAL, ShaderAttribute::Buffer::VERTEX},
		{ ShaderAttribute::Type::VEC3, ShaderAttribute::Purpose::TRANSLATION, ShaderAttribute::Buffer::INSTANCED},
	};

	Instances instances;
	for (std::uint32_t x = 0; x < 100; x++)
	{
		for (std::uint32_t y = 0; y < 100; y++)
		{
			auto pos = glm::vec3{ x, 0, y};
			instances.push_back(pos.x);
			instances.push_back(pos.y);
			instances.push_back(pos.z);
		}
	}

	auto instanced_vertex_array = blue::Context::gpu_system().submit(CreateInstancedMeshEntity{ vertex_array, instanced_attributes, instances, 100 * 100 }).get();

	// Create environment

	auto environment_future = blue::Context::gpu_system().submit(CreateEnvironmentEntity{});
	environment_future.wait();
	auto environment = environment_future.get();

	PerspectiveCamera camera;

	blue::Context::gpu_system().submit(UpdateEnvironmentEntity_Projection{ environment, camera.get_projection() });
	blue::Context::gpu_system().submit(UpdateEnvironmentEntity_View{ environment, camera.get_view() });

	// Submit render command consisting of compiled shader and uploaded mesh

	RenderEntity entity;
	entity.position = { 0, 0, -2.5f };
	entity.shader = shader;
	entity.vertex_array = instanced_vertex_array;
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
