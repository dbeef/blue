#include <blue/Context.hpp>
#include <blue/Timestep.hpp>
#include <blue/ShaderUtils.h>
#include <blue/TextureUtils.hpp>
#include <blue/camera/OrthographicCamera.hpp>
#include <blue/FontUtils.hpp>

#include <atomic>
#include <string>
#include <cstdint>
#include <cstdlib>

namespace Sample
{
	const std::uint16_t window_width = 400;
	const std::uint16_t window_height = 400;
}

int main(int argc, char* argv[])
{
	blue::Context::init();
	blue::Context::window().create(Sample::window_width, Sample::window_height);
	blue::Context::gpu_thread().run();

	std::atomic_bool running{ true };

	blue::Context::input().registerKeyCallback({
													   [&running]()
													   { running = false; },
													   SDLK_ESCAPE,
													   SDL_KEYDOWN
		}
	);

	auto compile_shader_entity = ShaderUtils::make_entity
	(
		"resources/SimpleTextureShader.vertex.glsl",
		"resources/SimpleTextureShader.fragment.glsl"
	);
	auto shader = blue::Context::gpu_system().submit(compile_shader_entity).get();

	// Upload mesh:
	
	Vertices vertices =
	{
		/* Vertex pos */ -0.5f, -0.5f, /* Tex coord */ 0.0f, 0.0f,
		/* Vertex pos */ -0.5f, 0.5f,  /* Tex coord */ 0.0f, 1.0f,
		/* Vertex pos */ 0.5f, 0.5f,   /* Tex coord */ 1.0f, 1.0f,
		/* Vertex pos */ 0.5f, -0.5f,  /* Tex coord */ 1.0f, 0.0f,
	};

	Indices indices =
	{
			0, 1, 2, 2, 3, 0
	};

	Attributes attributes =
	{
			{ShaderAttribute::Type::VEC2, ShaderAttribute::Purpose::VERTEX_POSITION,    ShaderAttribute::Buffer::VERTEX},
			{ShaderAttribute::Type::VEC2, ShaderAttribute::Purpose::TEXTURE_COORDINATE, ShaderAttribute::Buffer::VERTEX}
	};

	auto vertex_array = blue::Context::gpu_system().submit(CreateMeshEntity{ vertices, indices, attributes, static_cast<std::uint32_t>(indices.size()) }).get();

	// Create environment:

	auto environment = blue::Context::gpu_system().submit(CreateEnvironmentEntity{}).get();

	// Upload camera's matrices:

	OrthographicCamera camera(OrthographicCamera::Mode::SCREEN_SPACE, blue::Context::window().get_width(), blue::Context::window().get_height());

	blue::Context::gpu_system().submit(UpdateEnvironmentEntity_Projection{ environment, camera.get_projection() });
	blue::Context::gpu_system().submit(UpdateEnvironmentEntity_View{ environment, camera.get_view() });

	// Load font and create texture with passed text:
	auto font = FontUtils::read_ttf_relative("resources/Lato-Black.ttf");
	auto create_texture_entity = FontUtils::create_text(font, "The quick brown fox jumps over the lazy dog.", Sample::window_width, Sample::window_height, 22);
	auto texture = blue::Context::gpu_system().submit(create_texture_entity).get();

	// Now set used texture slot in shader:

	UpdateUniformVariableEntity update_uniform_entity{};
	update_uniform_entity.type = ShaderAttribute::Type::INT;
	update_uniform_entity.value = &create_texture_entity.slot;
	update_uniform_entity.program = shader;
	update_uniform_entity.location = 9;

	blue::Context::gpu_system().submit(update_uniform_entity);

	// Submit render command consisting of compiled shader, uploaded mesh and following geometry properties:
	RenderEntity entity;
	entity.position = { blue::Context::window().get_width() / 2, blue::Context::window().get_height() / 2, 0.0f };
	entity.shader = shader;
	entity.vertex_array = vertex_array;
	entity.scale = { Sample::window_width, Sample::window_height, 1.0f };
	entity.rotation = glm::identity<glm::quat>();
	entity.environment = environment;
	entity.textures[0] = texture;

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

	return EXIT_SUCCESS;
}
