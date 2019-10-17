#include <blue/Context.hpp>
#include <blue/Timestep.hpp>
#include <blue/ShaderUtils.h>
#include <blue/ModelLoader.h>
#include <blue/camera/PerspectiveCamera.hpp>
#include <blue/camera/OrthographicCamera.hpp>

#include <cmath>
#include <atomic>

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
    auto compile_depth_shader_entity = ShaderUtils::make_entity("resources/Triangle.vertex.glsl", "resources/Triangle.fragment.glsl");
	auto depth_shader = blue::Context::gpu_system().submit(compile_depth_shader_entity).get();

	// Issue the GPU thread with task of uploading mesh:

	Attributes attributes =
	{
		{ ShaderAttribute::Type::VEC3, ShaderAttribute::Purpose::VERTEX_POSITION, ShaderAttribute::Buffer::VERTEX},
		{ ShaderAttribute::Type::VEC3, ShaderAttribute::Purpose::COLOR, ShaderAttribute::Buffer::VERTEX},
		{ ShaderAttribute::Type::VEC3, ShaderAttribute::Purpose::NORMAL, ShaderAttribute::Buffer::VERTEX}
	};

	// Simple model I created, that utilizes vertex painting.
	auto scene_ptr = models::load_scene("resources/PineTree.fbx");
	unsigned int tree_vertex_counter = 0;
	auto tree_vertices = models::parse_scene(scene_ptr, attributes, tree_vertex_counter);
	auto tree_vertex_array = blue::Context::gpu_system().submit(CreateMeshEntity{ tree_vertices, {}, attributes, tree_vertex_counter }).get();

	// Floor quad.
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

	auto framebuffer = blue::Context::gpu_system().submit(CreateFramebufferEntity{ true, FramebufferAttachmentType::DEPTH_ATTACHMENT, 1024, 1024 }).get();

	// Create environment

	auto environment = blue::Context::gpu_system().submit(CreateEnvironmentEntity{}).get();

	PerspectiveCamera camera;
	camera.set_far(500.0f);
	camera.set_near(0.1f);
	camera.set_pos({ 0, 0, 10.0f });

	blue::Context::gpu_system().submit(UpdateEnvironmentEntity_Projection{ environment, camera.get_projection() });
	blue::Context::gpu_system().submit(UpdateEnvironmentEntity_View{ environment, camera.get_view() });

	// Create light source environment

	auto light_environment = blue::Context::gpu_system().submit(CreateEnvironmentEntity{}).get();

	OrthographicCamera ortho(OrthographicCamera::Mode::CLIP_SPACE);
	ortho.set_far(500.0f);
	ortho.set_near(-100.0f);

	// Upload light-space matrices to both environments:
	blue::Context::gpu_system().submit(UpdateEnvironmentEntity_Projection{ light_environment, ortho.get_projection() });
	blue::Context::gpu_system().submit(UpdateEnvironmentEntity_View{ light_environment, ortho.get_view() });
	blue::Context::gpu_system().submit(UpdateEnvironmentEntity_LightSpaceMatrix{ environment, ortho.get_projection() * ortho.get_view() });

	// Submit render command consisting of compiled shader and uploaded mesh

	RenderEntity tree_entity;
	tree_entity.position = { 0.0f, -2.5f, -5.5f };
	tree_entity.shader = shader;
	tree_entity.vertex_array = tree_vertex_array;
	tree_entity.scale = 0.5f;
	tree_entity.rotation = { 0, 0, 0, 0 };
	tree_entity.environment = environment;
	tree_entity.texture = framebuffer.texture;
	tree_entity.framebuffer.framebuffer = 0;
	tree_entity.id = blue::Context::renderer().add(tree_entity);

	RenderEntity tree_shadow_entity;
	tree_shadow_entity.position = tree_entity.position;
	tree_shadow_entity.shader = depth_shader;
	tree_shadow_entity.vertex_array = tree_vertex_array;
	tree_shadow_entity.scale = 0.5f;
	tree_shadow_entity.rotation = { 0, 0, 0, 0 };
	tree_shadow_entity.framebuffer = framebuffer;
	tree_shadow_entity.environment = light_environment;
	tree_shadow_entity.texture = 0;
	tree_shadow_entity.id = blue::Context::renderer().add(tree_shadow_entity);

	RenderEntity floor_entity;
	floor_entity.position = { 0.0f, 0.0f, -25.0f };
	floor_entity.shader = shader;
	floor_entity.vertex_array = floor_vertex_array;
	floor_entity.scale = 100.0f;
	floor_entity.rotation = { 0, 0, 0, 0 };
	floor_entity.environment = environment;
	floor_entity.texture = framebuffer.texture;
	floor_entity.framebuffer.framebuffer = 0;
	floor_entity.id = blue::Context::renderer().add(floor_entity);

	// Start logics loop with timestep limited to 30 times per second:
	Timestep timestep(30);

	const float r = 10.0f;
    float x = 0.0f;

	while (running)
	{
		timestep.mark_start();
		blue::Context::input().poll();

		// Move camera in a circle with tree in the center

		x += 0.01f;
		float camera_x = std::sin(x) * r;
		float camera_y = std::cos(x) * r;

        camera.set_pos({camera_x, 0, camera_y});
        camera.look_at({0.0f, -2.5f, -5.5f});

        blue::Context::gpu_system().submit(UpdateEnvironmentEntity_View{ environment, camera.get_view() });

		timestep.mark_end();
		timestep.delay();
	}

	blue::Context::gpu_thread().stop();
	blue::Context::dispose();

	return 0;
}
