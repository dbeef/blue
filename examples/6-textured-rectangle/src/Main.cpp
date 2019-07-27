#include <blue/Context.hpp>
#include <blue/Timestep.hpp>
#include <blue/ShaderUtils.h>
#include <blue/TextureUtils.hpp>
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

	Vertices vertices =
	{
		/* Vertex pos */ -1.0, -1.0, 0.0f, /* Tex coord */ 0.0f, 0.0f,
		/* Vertex pos */ -1.0, 1.0, 0.0f,  /* Tex coord */ 0.0f, 1.0f,
		/* Vertex pos */ 1.0, 1.0, 0.0f,   /* Tex coord */ 1.0f, 1.0f,
		/* Vertex pos */ 1.0, -1.0, 0.0f,  /* Tex coord */ 1.0f, 0.0f,
	};

	Indices indices =
	{
		0, 1, 2, 2, 3, 0
	};

	Attributes attributes =
	{
		{ ShaderAttribute::Type::VEC3, ShaderAttribute::Purpose::VERTEX_POSITION},
		{ ShaderAttribute::Type::VEC2, ShaderAttribute::Purpose::TEXTURE_COORDINATE}
	};

	auto vertex_array_future = blue::Context::gpu_system().submit(CreateMeshEntity{ vertices, indices, attributes, 6 });
	vertex_array_future.wait();
	auto vertex_array = vertex_array_future.get();

	// Create environment

	auto environment_future = blue::Context::gpu_system().submit(CreateEnvironmentEntity{});
	environment_future.wait();
	auto environment_id = environment_future.get();

	// Upload camera's matrices

	PerspectiveCamera camera;
	camera.cameraPos.x = 0.0f;
	camera.cameraPos.y = 0.0f;
	camera.cameraPos.z = 10.0f;

	blue::Context::gpu_system().submit(UpdateEnvironmentEntity_Projection{ environment_id, camera.get_projection() });
	blue::Context::gpu_system().submit(UpdateEnvironmentEntity_View{ environment_id, camera.get_view() });

	// Create texture

	auto create_texture_entity = ImageUtils::read("resources/blue.png");
	auto texture_future = blue::Context::gpu_system().submit(create_texture_entity);
	texture_future.wait();
	auto texture = texture_future.get();

	// Submit render command consisting of compiled shader, uploaded mesh and following geometry properties:

	RenderEntity entity;
	entity.position = { 0, 0, -2.5f };
	entity.shader = shader;
	entity.vertex_array = vertex_array;
	entity.scale = 2.0f;
	entity.rotation = { 0, 0, 0, 0 };
	entity.environment = environment_id;
	entity.texture = texture;

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
