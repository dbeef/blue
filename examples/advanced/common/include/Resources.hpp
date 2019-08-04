#pragma once

#include "blue/gpu/GpuEntities.hpp"
#include "blue/camera/PerspectiveCamera.hpp"

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
		UniformBufferId environment = 0;
		PerspectiveCamera camera;
	} map_environment;

	struct {
		ShaderId clickable_map_shader;
		ShaderId decoration_map_shader;
		ShaderId model_shader;
	} shaders;
	
	struct {
		VertexArray pine_tree;
		VertexArray hurdle;
		VertexArray wheat;
		VertexArray boulder;
		VertexArray small_boulder;
		VertexArray grass;
		VertexArray pylon;
		VertexArray bush;
		VertexArray cut_tree;
		VertexArray track;
		VertexArray bridge;
	} models;

private:

	static Resources* _instance;
};
