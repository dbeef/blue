#pragma once

#include "blue/gpu/GpuEntities.hpp"

class Resources
{
public:

	static Resources& instance();

	static void init();

	static void dispose();

	void load_shaders();

	void load_models();

	void load_textures();

	struct {
		ShaderId clickable_map_shader;
	} shaders;

private:

	static Resources* _instance;
};
