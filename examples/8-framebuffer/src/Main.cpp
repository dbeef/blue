#include <blue/Context.hpp>
#include <blue/Timestep.hpp>
#include <blue/ShaderUtils.h>
#include <blue/ModelLoader.h>
#include <blue/camera/PerspectiveCamera.hpp>
#include <blue/camera/OrthographicCamera.hpp>

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
	
	auto depth_compile_shader_entity = ShaderUtils::make_entity("resources/SimpleDepth.vertex.glsl", "resources/SimpleDepth.fragment.glsl");
	auto depth_shader = blue::Context::gpu_system().submit(depth_compile_shader_entity).get();

	// Issue the GPU thread with task of uploading mesh:

	Attributes attributes =
	{
		{ ShaderAttribute::Type::VEC3, ShaderAttribute::Purpose::VERTEX_POSITION, ShaderAttribute::Buffer::VERTEX},
		{ ShaderAttribute::Type::VEC3, ShaderAttribute::Purpose::COLOR, ShaderAttribute::Buffer::VERTEX},
		{ ShaderAttribute::Type::VEC3, ShaderAttribute::Purpose::NORMAL, ShaderAttribute::Buffer::VERTEX}
	};

	// Simple model I created, that utilizes vertex painting.
	auto scene_ptr = models::load_scene("resources/PineTree.fbx");
	unsigned int vertex_counter = 0;
	auto vertices = models::parse_scene(scene_ptr, attributes, vertex_counter);

	auto vertex_array = blue::Context::gpu_system().submit(CreateMeshEntity{ vertices, {}, attributes, vertex_counter}).get();

	// Floor
	
	Vertices floor_vertices =
	{
		/* Vertex pos */-1.0f, -1.0f, 0.0f, /* Color */ 1.0f, 1.0f, 1.0f, /* Normal */ 0.0f, 0.0f, 1.0f,
		/* Vertex pos */-1.0f,  1.0f, 0.0f, /* Color */ 1.0f, 1.0f, 1.0f, /* Normal */ 0.0f, 0.0f, 1.0f,
		/* Vertex pos */ 1.0f,  1.0f, 0.0f, /* Color */ 1.0f, 1.0f, 1.0f, /* Normal */ 0.0f, 0.0f, 1.0f,
		/* Vertex pos */ 1.0f, -1.0f, 0.0f, /* Color */ 1.0f, 1.0f, 1.0f, /* Normal */ 0.0f, 0.0f, 1.0f,
	};

	Indices floor_indices =
	{
		0, 1, 2, 2, 3, 0
	};

	auto floor_vertex_array = blue::Context::gpu_system().submit(CreateMeshEntity{ floor_vertices, floor_indices, attributes, 6 }).get();

	// Create framebuffer with depth component only

	auto framebuffer = blue::Context::gpu_system().submit(CreateFramebufferEntity{ true, 1024, 1024 }).get();

	// Create environment

	auto environment = blue::Context::gpu_system().submit(CreateEnvironmentEntity{}).get();

	PerspectiveCamera camera;

	blue::Context::gpu_system().submit(UpdateEnvironmentEntity_Projection{ environment, camera.get_projection() });
	blue::Context::gpu_system().submit(UpdateEnvironmentEntity_View{ environment, camera.get_view() });

	// Create light source environment

	auto light_environment = blue::Context::gpu_system().submit(CreateEnvironmentEntity{}).get();

	OrthographicCamera ortho;
	blue::Context::gpu_system().submit(UpdateEnvironmentEntity_Projection{ light_environment, ortho.get_projection() });
	blue::Context::gpu_system().submit(UpdateEnvironmentEntity_View{ light_environment, ortho.get_view() });

	// Submit render command consisting of compiled shader and uploaded mesh

	RenderEntity entity;
	entity.position = { -2.0f,-2.0, -2.5f };
	entity.shader = shader;
	entity.vertex_array = vertex_array;
	entity.scale = 0.5f;
	entity.rotation = { 0, 0, 0, 0 };
	entity.environment = environment;
	entity.id = blue::Context::renderer().add(entity);
/*
	RenderEntity shadow_entity;
	shadow_entity.position = { -2.0f,-2.0, -2.5f };
	shadow_entity.shader = depth_shader;
	shadow_entity.vertex_array = vertex_array;
	shadow_entity.scale = 0.5f;
	shadow_entity.rotation = { 0, 0, 0, 0 };
	shadow_entity.framebuffer = framebuffer;
	shadow_entity.environment = light_environment;
	shadow_entity.id = blue::Context::renderer().add(shadow_entity);*/

	RenderEntity floor_entity;
	floor_entity.position = { 0.0f, 0.0f, -25.0f };
	floor_entity.shader = shader;
	floor_entity.vertex_array = floor_vertex_array;
	floor_entity.scale = 10.0f;
	floor_entity.rotation = { 0, 0, 0, 0 };
	floor_entity.environment = environment;
	floor_entity.id = blue::Context::renderer().add(floor_entity);

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
