#include "jobs/FramerateRenderingJob.h"
#include "blue/Timestep.hpp"
#include "Game.hpp"
#include "Resources.hpp"
#include "blue/camera/PerspectiveCamera.hpp"

#include <limits>

void FramerateRenderingJob::shutdown()
{
	_running.store(false);
	_intersection_thread.join();
}

void FramerateRenderingJob::start()
{
	// Create quad:

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

	// Now set used texture slot in shader:

	std::uint32_t slot = 3;

	UpdateUniformVariableEntity update_uniform_entity{};
	update_uniform_entity.type = ShaderAttribute::Type::INT;
	update_uniform_entity.value = &slot;
	update_uniform_entity.program = Resources::instance().shaders.simple_texture;
	update_uniform_entity.location = 0;
	update_uniform_entity.name = "sampler";

	blue::Context::gpu_system().submit(update_uniform_entity);
	blue::Context::gpu_thread().wait_for_render_pass();

	// Spawn render entity:

	_entity.position = { blue::Context::window().get_width() / 2, blue::Context::window().get_height() / 2, 0.0f };
	_entity.shader = Resources::instance().shaders.simple_texture;
	_entity.vertex_array = vertex_array;
	_entity.scale = { 64, 64, 1.0f };
	_entity.position = { 0.5f * _entity.scale.x, 0.5f * _entity.scale.y, 0 };
	_entity.rotation = glm::identity<glm::quat>();
	_entity.environment = Resources::instance().gui_environment.environment;
	_entity.id = blue::Context::renderer().add(_entity);
	_entity.framebuffer.framebuffer = 0;

	_running.store(true);
	_intersection_thread = std::thread(&FramerateRenderingJob::framerate_rendering_loop, this);
}

void FramerateRenderingJob::framerate_rendering_loop()
{
	Timestep timestep(1.0f);

	while (_running.load())
	{
		timestep.mark_start();

		if (_entity.textures[0].id)
		{
			blue::Context::gpu_system().submit(DisposeTextureEntity{ _entity.textures[0] });
		}

		auto create_texture_entity = FontUtils::create_text
		(
			Resources::instance().fonts.lato,
			std::to_string(static_cast<int>(1000.0f * 1000.0f / blue::Context::gpu_thread().get_time_spent())) + " FPS",
            64,
			64,
			16
		);
		create_texture_entity.slot = 3;

        _entity.textures[0] = blue::Context::gpu_system().submit(create_texture_entity).get();
		blue::Context::renderer().update(_entity);

		timestep.mark_end();
		timestep.delay();
	}
}
