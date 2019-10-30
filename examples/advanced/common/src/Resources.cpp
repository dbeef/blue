#include <blue/Assertions.h>
#include <blue/ShaderUtils.h>
#include <blue/ModelLoader.h>
#include <blue/Context.hpp>
#include <blue/TextureUtils.hpp>
#include <Resources.hpp>

#include "Resources.hpp"

Resources* Resources::_instance = nullptr;

Resources& Resources::instance()
{
	BLUE_ASSERT(_instance);
	return *_instance;
}

void Resources::init()
{
	BLUE_ASSERT(!_instance);
	_instance = new Resources();
}

void Resources::dispose()
{
	BLUE_ASSERT(_instance);
	delete _instance;
	_instance = nullptr;
}

void Resources::load_shaders()
{
	{
		auto entity = ShaderUtils::make_entity("resources/SimpleDepth.vertex.glsl", "resources/SimpleDepth.fragment.glsl");
		auto shader = blue::Context::gpu_system().submit(entity).get();
		shaders.simple_depth = shader;
		BLUE_ASSERT(shader > 0);
	}
	{
		auto entity = ShaderUtils::make_entity("resources/TileHighlight.vertex.glsl", "resources/TileHighlight.fragment.glsl");
		auto shader = blue::Context::gpu_system().submit(entity).get();
		shaders.tile_highlight = shader;
		BLUE_ASSERT(shader > 0);
	}
	{
		auto entity = ShaderUtils::make_entity("resources/SimpleDepth_Instanced.vertex.glsl", "resources/SimpleDepth.fragment.glsl");
		auto shader = blue::Context::gpu_system().submit(entity).get();
		shaders.simple_depth_instanced = shader;
		BLUE_ASSERT(shader > 0);
	}
	{
		auto entity = ShaderUtils::make_entity("resources/Decoration.vertex.glsl", "resources/Decoration.fragment.glsl");
		auto shader = blue::Context::gpu_system().submit(entity).get();
		shaders.decoration_map = shader;
		BLUE_ASSERT(shader > 0);
	}
	{
		auto entity = ShaderUtils::make_entity("resources/Clickable.vertex.glsl", "resources/Clickable.fragment.glsl");
		auto shader = blue::Context::gpu_system().submit(entity).get();
		shaders.clickable_map = shader;
		BLUE_ASSERT(shader > 0);
	}
	{
		auto entity = ShaderUtils::make_entity("resources/Water.vertex.glsl", "resources/Water.fragment.glsl");
		auto shader = blue::Context::gpu_system().submit(entity).get();
		shaders.water = shader;
		BLUE_ASSERT(shader > 0);
	}
	{
		auto entity = ShaderUtils::make_entity("resources/Model.vertex.glsl", "resources/Model.fragment.glsl");
		auto shader = blue::Context::gpu_system().submit(entity).get();
		shaders.model = shader;
		BLUE_ASSERT(shader > 0);
	}
	{
		auto entity = ShaderUtils::make_entity("resources/Model_Instanced.vertex.glsl", "resources/Model_Instanced.fragment.glsl");
		auto shader = blue::Context::gpu_system().submit(entity).get();
		shaders.model_instanced = shader;
		BLUE_ASSERT(shader > 0);
	}
	{
		auto entity = ShaderUtils::make_entity("resources/Swinging_Instanced.vertex.glsl", "resources/Swinging_Instanced.fragment.glsl");
		auto shader = blue::Context::gpu_system().submit(entity).get();
		shaders.swinging = shader;
		BLUE_ASSERT(shader > 0);
	}
}

void Resources::load_render_entities()
{
	// TODO: This should be removed; render entities are to be created by specific state (extending BaseState)
    {
        RenderEntity entity;
        const auto& environment = Resources::instance().map_environment.environment;

        entity.position = { 100.0f, 100.0f, 100.0f };
        entity.shader = Resources::instance().shaders.tile_highlight;
        entity.rotation = {};
        entity.environment = environment;
        entity.scale = {1.0f,1.0f,1.0f};
        entity.framebuffer.framebuffer = 0;
        entity.vertex_array = Resources::instance().models.square;
        entity.id = blue::Context::renderer().add(entity);

        render_entities.selected_tile_highlight = entity;
    }
}

void Resources::load_models()
{
	Attributes attributes =
	{
		{ ShaderAttribute::Type::VEC3, ShaderAttribute::Purpose::VERTEX_POSITION, ShaderAttribute::Buffer::VERTEX},
		{ ShaderAttribute::Type::VEC3, ShaderAttribute::Purpose::NORMAL, ShaderAttribute::Buffer::VERTEX},
		{ ShaderAttribute::Type::VEC3, ShaderAttribute::Purpose::COLOR, ShaderAttribute::Buffer::VERTEX},
	};

	Attributes swinging_attributes =
	{
		{ ShaderAttribute::Type::VEC3, ShaderAttribute::Purpose::VERTEX_POSITION, ShaderAttribute::Buffer::VERTEX},
		{ ShaderAttribute::Type::VEC3, ShaderAttribute::Purpose::NORMAL, ShaderAttribute::Buffer::VERTEX},
		{ ShaderAttribute::Type::VEC3, ShaderAttribute::Purpose::COLOR, ShaderAttribute::Buffer::VERTEX},
		{ ShaderAttribute::Type::FLOAT, ShaderAttribute::Purpose::NORMALIZED_HEIGHT, ShaderAttribute::Buffer::VERTEX},
	};

	{
		auto scene_ptr = models::load_scene("resources/PineTree.fbx");
		unsigned int vertex_counter = 0;
		auto vertices = models::parse_scene(scene_ptr, swinging_attributes, vertex_counter);

		auto vertex_array = blue::Context::gpu_system().submit(CreateMeshEntity{ vertices, {}, swinging_attributes, vertex_counter }).get();
		models.pine_tree = vertex_array;
	}
	{
		auto scene_ptr = models::load_scene("resources/Hurdle.fbx");
		unsigned int vertex_counter = 0;
		auto vertices = models::parse_scene(scene_ptr, attributes, vertex_counter);

		auto vertex_array = blue::Context::gpu_system().submit(CreateMeshEntity{ vertices, {}, attributes, vertex_counter }).get();
		models.hurdle = vertex_array;
	}
	{
		auto scene_ptr = models::load_scene("resources/Wheat.fbx");
		unsigned int vertex_counter = 0;
		auto vertices = models::parse_scene(scene_ptr, attributes, vertex_counter);

		auto vertex_array = blue::Context::gpu_system().submit(CreateMeshEntity{ vertices, {}, attributes, vertex_counter }).get();
		models.wheat = vertex_array;
	}
	{
		auto scene_ptr = models::load_scene("resources/Boulder.fbx");
		unsigned int vertex_counter = 0;
		auto vertices = models::parse_scene(scene_ptr, attributes, vertex_counter);

		auto vertex_array = blue::Context::gpu_system().submit(CreateMeshEntity{ vertices, {}, attributes, vertex_counter }).get();
		models.boulder = vertex_array;
	}
	{
		auto scene_ptr = models::load_scene("resources/SmallBoulder.fbx");
		unsigned int vertex_counter = 0;
		auto vertices = models::parse_scene(scene_ptr, attributes, vertex_counter);

		auto vertex_array = blue::Context::gpu_system().submit(CreateMeshEntity{ vertices, {}, attributes, vertex_counter }).get();
		models.small_boulder = vertex_array;
	}
	{
		auto scene_ptr = models::load_scene("resources/Grass.fbx");
		unsigned int vertex_counter = 0;
		auto vertices = models::parse_scene(scene_ptr, attributes, vertex_counter);

		auto vertex_array = blue::Context::gpu_system().submit(CreateMeshEntity{ vertices, {}, attributes, vertex_counter }).get();
		models.grass = vertex_array;
	}
	{
		auto scene_ptr = models::load_scene("resources/Pylon.fbx");
		unsigned int vertex_counter = 0;
		auto vertices = models::parse_scene(scene_ptr, attributes, vertex_counter);

		auto vertex_array = blue::Context::gpu_system().submit(CreateMeshEntity{ vertices, {}, attributes, vertex_counter }).get();
		models.pylon = vertex_array;
	}
	{
		auto scene_ptr = models::load_scene("resources/Bush.fbx");
		unsigned int vertex_counter = 0;
		auto vertices = models::parse_scene(scene_ptr, attributes, vertex_counter);

		auto vertex_array = blue::Context::gpu_system().submit(CreateMeshEntity{ vertices, {}, attributes, vertex_counter }).get();
		models.bush = vertex_array;
	}
	{
		auto scene_ptr = models::load_scene("resources/CutTree.fbx");
		unsigned int vertex_counter = 0;
		auto vertices = models::parse_scene(scene_ptr, attributes, vertex_counter);

		auto vertex_array = blue::Context::gpu_system().submit(CreateMeshEntity{ vertices, {}, attributes, vertex_counter }).get();
		models.cut_tree = vertex_array;
	}
	{
		auto scene_ptr = models::load_scene("resources/Track.fbx");
		unsigned int vertex_counter = 0;
		auto vertices = models::parse_scene(scene_ptr, attributes, vertex_counter);

		auto vertex_array = blue::Context::gpu_system().submit(CreateMeshEntity{ vertices, {}, attributes, vertex_counter }).get();
		models.track = vertex_array;
	}
	{
		auto scene_ptr = models::load_scene("resources/Bridge.fbx");
		unsigned int vertex_counter = 0;
		auto vertices = models::parse_scene(scene_ptr, attributes, vertex_counter);

		auto vertex_array = blue::Context::gpu_system().submit(CreateMeshEntity{ vertices, {}, attributes, vertex_counter }).get();
		models.bridge = vertex_array;
	}
    {
        Vertices vertices =
                {
                        /* Vertex pos */ -0.0f, 0.0f,  -0.0f,
                        /* Vertex pos */ -0.0f, 0.0f,  1.0f,
                        /* Vertex pos */ 1.0f,  0.0f,  1.0f,
                        /* Vertex pos */ 1.0f,  0.0f, -0.0f,
                };

        Indices indices =
                {
                        0, 1, 2, 2, 3, 0
                };

        Attributes attributes =
                {
                        { ShaderAttribute::Type::VEC3, ShaderAttribute::Purpose::VERTEX_POSITION, ShaderAttribute::Buffer::VERTEX},
                };

        models.square = blue::Context::gpu_system().submit(CreateMeshEntity{ vertices, indices, attributes, 6 }).get();
    }
}

void Resources::load_textures()
{
    {
        auto clicked_entity = CreateTextureEntity{ImageUtils::read("resources/button_start_clicked.png")};
        clicked_entity.slot = 5;
        textures.start_clicked = blue::Context::gpu_system().submit(clicked_entity).get();
        auto idle_entity = CreateTextureEntity{ImageUtils::read("resources/button_start.png")};
        idle_entity.slot = 6;
        textures.start = blue::Context::gpu_system().submit(idle_entity).get();
    }
    {
        auto clicked_entity = CreateTextureEntity{ImageUtils::read("resources/button_exit_clicked.png")};
        clicked_entity.slot = 5;
        textures.exit_clicked = blue::Context::gpu_system().submit(clicked_entity).get();
        auto idle_entity = CreateTextureEntity{ImageUtils::read("resources/button_exit.png")};
        idle_entity.slot = 6;
        textures.exit = blue::Context::gpu_system().submit(idle_entity).get();
    }
    {
        auto clicked_entity = CreateTextureEntity{ImageUtils::read("resources/button_options_clicked.png")};
        clicked_entity.slot = 5;
        textures.options_clicked = blue::Context::gpu_system().submit(clicked_entity).get();
        auto idle_entity = CreateTextureEntity{ImageUtils::read("resources/button_options.png")};
        idle_entity.slot = 6;
        textures.options = blue::Context::gpu_system().submit(idle_entity).get();
    }
}

void Resources::load_environment()
{
	// Cameras
	light_environment.camera = OrthographicCamera(OrthographicCamera::Mode::CLIP_SPACE, blue::Context::window().get_width(), blue::Context::window().get_height());
	map_environment.camera = PerspectiveCamera(blue::Context::window().get_width(), blue::Context::window().get_height());
	gui_environment.camera = OrthographicCamera(OrthographicCamera::Mode::SCREEN_SPACE, blue::Context::window().get_width(), blue::Context::window().get_height());

	// Environments
	map_environment.environment = blue::Context::gpu_system().submit(CreateEnvironmentEntity{}).get();
	light_environment.environment = blue::Context::gpu_system().submit(CreateEnvironmentEntity{}).get();
	gui_environment.environment = blue::Context::gpu_system().submit(CreateEnvironmentEntity{}).get();

	{
		// Create framebuffer with only depth component for shadows:
		light_environment.depth = blue::Context::gpu_system().submit(CreateFramebufferEntity{}).get();

		auto renderTexture = blue::Context::gpu_system().submit(CreateTextureEntity{
				std::make_shared<std::vector<char>>(), true,
				TextureFiltering::LINEAR,
				TextureWrapping::CLAMP_TO_EDGE,
				1024,
				1024,
				0,
				TexturePassedDataFormat::DEPTH_COMPONENT,
				TextureStoringFormat::DEPTH_COMPONENT,
				TexturePassedDataComponentSize::UNSIGNED_BYTE
		}).get();
		light_environment.depth.texture = renderTexture;
		blue::Context::gpu_system().submit(
				AddFramebufferTextureAttachmentEntity{light_environment.depth, FramebufferAttachmentType::DEPTH_ATTACHMENT}).wait();
	}

	// Upload map environment camera matrices
	blue::Context::gpu_system().submit(UpdateEnvironmentEntity_Projection{ map_environment.environment, map_environment.camera.get_projection() });
	blue::Context::gpu_system().submit(UpdateEnvironmentEntity_View{ map_environment.environment, map_environment.camera.get_view() });

	// Upload GUI environment camera matrices
	blue::Context::gpu_system().submit(UpdateEnvironmentEntity_Projection{gui_environment.environment, gui_environment.camera.get_projection()});
	blue::Context::gpu_system().submit(UpdateEnvironmentEntity_View{gui_environment.environment, gui_environment.camera.get_view()});

	{
		// Move light-source camera to initial position
		light_environment.camera.set_far(500.0f);
		light_environment.camera.set_near(-100.0f);
		light_environment.camera.set_pos({64.0f, 20.4532f, 64.0f});
		// FIXME: Add functionality to camera to look at specific point.
		// light_environment.camera.look_at({64.0f, 0.0f, 64.0f});

		// Upload light-space matrices to both environments:
		blue::Context::gpu_system().submit(
				UpdateEnvironmentEntity_Projection{light_environment.environment, light_environment.camera.get_projection()});
		blue::Context::gpu_system().submit(
				UpdateEnvironmentEntity_View{light_environment.environment, light_environment.camera.get_view()});

		blue::Context::gpu_system().submit(UpdateEnvironmentEntity_LightSpaceMatrix{
				map_environment.environment,
				light_environment.camera.get_projection() * light_environment.camera.get_view()
		});
		blue::Context::gpu_system().submit(
				UpdateEnvironmentEntity_LightPos{map_environment.environment, light_environment.camera.get_position()});
	}

	{
		// Move camera to initial position
		map_environment.camera.add_rotation(-68.5f, -60.25f);
		map_environment.camera.set_pos({32, 22, 32});
		//map_environment.camera.look_at({ 0, 0, 0 });
		blue::Context::gpu_system().submit(
				UpdateEnvironmentEntity_View{map_environment.environment, map_environment.camera.get_view()});
		blue::Context::gpu_system().submit(
				UpdateEnvironmentEntity_CameraPos{map_environment.environment, map_environment.camera.get_position()});
	}

	// Set pale foggy-white clear color
	blue::Context::gpu_system().submit(SetClearColorEntity{ {1.0f, 1.0f, 1.0f} });
}
