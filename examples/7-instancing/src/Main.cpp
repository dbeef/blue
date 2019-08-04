#include <blue/Context.hpp>
#include <blue/Timestep.hpp>
#include <blue/ShaderUtils.h>
#include <blue/camera/PerspectiveCamera.hpp>

#include <atomic>

int main(int argc, char* argv[])
{

	blue::Context::init();
	blue::Context::window().create(800, 600);
	blue::Context::gpu_thread().run();

	// Register ESC key callback: 
	std::atomic_bool running{ true };

	blue::Context::input().registerKeyCallback({
		[&running]() { running = false; },
			SDLK_ESCAPE,
			SDL_KEYDOWN
		}
	);

	// Now render thread is running and waiting for commands to process,
	// leaving this thread only for CPU logics. 

	// Issue the GPU thread with task of compiling shader program:

	auto compile_shader_entity = ShaderUtils::make_entity("resources/Triangle.vertex.glsl", "resources/Triangle.fragment.glsl");
	auto shader_future = blue::Context::gpu_system().submit(compile_shader_entity);
	shader_future.wait();
	auto shader = shader_future.get();

	// Issue the GPU thread with task of uploading mesh:

	std::uint32_t instances = 100;
	Instances instance_buffer;
	for (std::uint32_t i = 0; i < instances; i++)
	{
		auto pos = glm::vec3{ static_cast<float>(i) / 10 };
		if (i % 2) pos *= -1;
		instance_buffer.push_back(pos.x);
		instance_buffer.push_back(0);
		instance_buffer.push_back(0);
	}

	Vertices vertices =
	{
		-1.0f, -1.0f, 0.0f, // left - position
		1.0f, -1.0f, 0.0f,  // right - position
		0.0f,  1.0f, 0.0f,  // up - position
	};

	Attributes attributes =
	{
		{ ShaderAttribute::Type::VEC3, ShaderAttribute::Purpose::VERTEX_POSITION, ShaderAttribute::Buffer::VERTEX},
		{ ShaderAttribute::Type::VEC3, ShaderAttribute::Purpose::TRANSLATION, ShaderAttribute::Buffer::INDEX},
	};

	Indices indices =
	{
		0, 1, 2
	};

	auto vertex_array_future = blue::Context::gpu_system().submit(CreateMeshEntity{ vertices, indices, attributes, 3, instance_buffer });
	vertex_array_future.wait();
	auto vertex_array = vertex_array_future.get();

	// Create environment

	auto environment_future = blue::Context::gpu_system().submit(CreateEnvironmentEntity{});
	environment_future.wait();
	auto environment_id = environment_future.get();

	// Upload camera's matrices

	PerspectiveCamera camera;

	blue::Context::gpu_system().submit(UpdateEnvironmentEntity_Projection{ environment_id, camera.get_projection() });
	blue::Context::gpu_system().submit(UpdateEnvironmentEntity_View{ environment_id, camera.get_view() });

	// Submit render command consisting of compiled shader, uploaded mesh and following geometry properties:

	RenderEntity entity;
	entity.position = { 0, 0, -2.5f };
	entity.shader = shader;
	entity.vertex_array = vertex_array;
	entity.scale = 2.0f;
	entity.rotation = { 1.0f, 1.0f, 0, 0 };
	entity.environment = environment_id;

	RenderEntityId id = blue::Context::renderer().add(entity);

	// Start logics loop with timestep limited to 30 times per second:
	Timestep timestep(30);

	while (running)
	{
		timestep.mark_start();
		blue::Context::input().poll();
		timestep.mark_end();
		timestep.delay();
	}

	blue::Context::gpu_thread().stop();
	blue::Context::dispose();

	return 0;
}
