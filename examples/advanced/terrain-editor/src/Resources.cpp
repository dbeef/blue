#include "Resources.hpp"
#include "blue/Assertions.h"
#include "blue/ShaderUtils.h"
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
}

void Resources::load_models()
{
}

void Resources::load_textures()
{
}
