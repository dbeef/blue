#include "Resources.hpp"
#include "blue/Assertions.h"

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
}

void Resources::load_models()
{
}

void Resources::load_textures()
{
}
