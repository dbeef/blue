#include "Resources.hpp"
#include "blue/Assertions.h"
#include "blue/ShaderUtils.h"
#include "blue/ModelLoader.h"
#include "blue/Context.hpp"

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
		auto compile_shader_entity = ShaderUtils::make_entity("resources/Triangle.vertex.glsl", "resources/Triangle.fragment.glsl");
		auto shader_future = blue::Context::gpu_system().submit(compile_shader_entity);
		shader_future.wait();
		shaders.clickable_map_shader = shader_future.get();
	}
	{
		auto compile_shader_entity = ShaderUtils::make_entity("resources/Decoration.vertex.glsl", "resources/Decoration.fragment.glsl");
		auto shader_future = blue::Context::gpu_system().submit(compile_shader_entity);
		shader_future.wait();
		shaders.decoration_map_shader = shader_future.get();
	}
	{
		auto compile_shader_entity = ShaderUtils::make_entity("resources/Model.vertex.glsl", "resources/Model.fragment.glsl");
		auto shader_future = blue::Context::gpu_system().submit(compile_shader_entity);
		shader_future.wait();
		shaders.model_shader = shader_future.get();
	}
}

void Resources::load_models()
{
	Attributes attributes =
	{
		{ ShaderAttribute::Type::VEC3, ShaderAttribute::Purpose::VERTEX_POSITION, ShaderAttribute::Buffer::VERTEX},
		{ ShaderAttribute::Type::VEC3, ShaderAttribute::Purpose::COLOR, ShaderAttribute::Buffer::VERTEX},
		{ ShaderAttribute::Type::VEC3, ShaderAttribute::Purpose::NORMAL, ShaderAttribute::Buffer::VERTEX}
	};

	{
		auto scene_ptr = models::load_scene("resources/PineTree.fbx");
		unsigned int vertex_counter = 0;
		auto vertices = models::parse_scene(scene_ptr, attributes, vertex_counter);

		auto vertex_array_future = blue::Context::gpu_system().submit(CreateMeshEntity{ vertices, {}, attributes, vertex_counter });
		vertex_array_future.wait();
		models.pine_tree = vertex_array_future.get();
	}
	{
		auto scene_ptr = models::load_scene("resources/Tank.fbx");
		unsigned int vertex_counter = 0;
		auto vertices = models::parse_scene(scene_ptr, attributes, vertex_counter);

		auto vertex_array_future = blue::Context::gpu_system().submit(CreateMeshEntity{ vertices, {}, attributes, vertex_counter });
		vertex_array_future.wait();
		models.tank = vertex_array_future.get();
	}
	{
		auto scene_ptr = models::load_scene("resources/Hurdle.fbx");
		unsigned int vertex_counter = 0;
		auto vertices = models::parse_scene(scene_ptr, attributes, vertex_counter);

		auto vertex_array_future = blue::Context::gpu_system().submit(CreateMeshEntity{ vertices, {}, attributes, vertex_counter });
		vertex_array_future.wait();
		models.hurdle = vertex_array_future.get();
	}
	{
		auto scene_ptr = models::load_scene("resources/Wheat.fbx");
		unsigned int vertex_counter = 0;
		auto vertices = models::parse_scene(scene_ptr, attributes, vertex_counter);

		auto vertex_array_future = blue::Context::gpu_system().submit(CreateMeshEntity{ vertices, {}, attributes, vertex_counter });
		vertex_array_future.wait();
		models.wheat = vertex_array_future.get();
	}
	{
		auto scene_ptr = models::load_scene("resources/Boulder.fbx");
		unsigned int vertex_counter = 0;
		auto vertices = models::parse_scene(scene_ptr, attributes, vertex_counter);

		auto vertex_array_future = blue::Context::gpu_system().submit(CreateMeshEntity{ vertices, {}, attributes, vertex_counter });
		vertex_array_future.wait();
		models.boulder = vertex_array_future.get();
	}
	{
		auto scene_ptr = models::load_scene("resources/SmallBoulder.fbx");
		unsigned int vertex_counter = 0;
		auto vertices = models::parse_scene(scene_ptr, attributes, vertex_counter);

		auto vertex_array_future = blue::Context::gpu_system().submit(CreateMeshEntity{ vertices, {}, attributes, vertex_counter });
		vertex_array_future.wait();
		models.small_boulder = vertex_array_future.get();
	}
	{
		auto scene_ptr = models::load_scene("resources/Grass.fbx");
		unsigned int vertex_counter = 0;
		auto vertices = models::parse_scene(scene_ptr, attributes, vertex_counter);

		auto vertex_array_future = blue::Context::gpu_system().submit(CreateMeshEntity{ vertices, {}, attributes, vertex_counter });
		vertex_array_future.wait();
		models.grass = vertex_array_future.get();
	}
	{
		auto scene_ptr = models::load_scene("resources/Pylon.fbx");
		unsigned int vertex_counter = 0;
		auto vertices = models::parse_scene(scene_ptr, attributes, vertex_counter);

		auto vertex_array_future = blue::Context::gpu_system().submit(CreateMeshEntity{ vertices, {}, attributes, vertex_counter });
		vertex_array_future.wait();
		models.pylon = vertex_array_future.get();
	}
	{
		auto scene_ptr = models::load_scene("resources/Bush.fbx");
		unsigned int vertex_counter = 0;
		auto vertices = models::parse_scene(scene_ptr, attributes, vertex_counter);

		auto vertex_array_future = blue::Context::gpu_system().submit(CreateMeshEntity{ vertices, {}, attributes, vertex_counter });
		vertex_array_future.wait();
		models.bush = vertex_array_future.get();
	}
	{
		auto scene_ptr = models::load_scene("resources/CutTree.fbx");
		unsigned int vertex_counter = 0;
		auto vertices = models::parse_scene(scene_ptr, attributes, vertex_counter);

		auto vertex_array_future = blue::Context::gpu_system().submit(CreateMeshEntity{ vertices, {}, attributes, vertex_counter });
		vertex_array_future.wait();
		models.cut_tree = vertex_array_future.get();
	}
	{
		auto scene_ptr = models::load_scene("resources/Track.fbx");
		unsigned int vertex_counter = 0;
		auto vertices = models::parse_scene(scene_ptr, attributes, vertex_counter);

		auto vertex_array_future = blue::Context::gpu_system().submit(CreateMeshEntity{ vertices, {}, attributes, vertex_counter });
		vertex_array_future.wait();
		models.track = vertex_array_future.get();
	}
	{
		auto scene_ptr = models::load_scene("resources/Bridge.fbx");
		unsigned int vertex_counter = 0;
		auto vertices = models::parse_scene(scene_ptr, attributes, vertex_counter);

		auto vertex_array_future = blue::Context::gpu_system().submit(CreateMeshEntity{ vertices, {}, attributes, vertex_counter });
		vertex_array_future.wait();
		models.bridge = vertex_array_future.get();
	}
}

void Resources::load_textures()
{
}
