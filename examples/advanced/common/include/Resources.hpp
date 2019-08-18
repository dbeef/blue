#pragma once

#include <blue/camera/OrthographicCamera.hpp>
#include "blue/gpu/GpuEntities.hpp"
#include "blue/camera/PerspectiveCamera.hpp"

enum class Model : int
{
    PINE_TREE = 0,
    HURDLE = 1,
    WHEAT = 2,
    BOULDER = 3,
    SMALL_BOULDER = 4,
    GRASS = 5,
    PYLON = 6,
    BUSH = 7,
    CUT_TREE = 8,
    TRACK = 9,
    BRIDGE = 10,
};

class Resources
{
public:

	static Resources& instance();

	static void init();

	static void dispose();

	void load_environment();

	void load_shaders();

	void load_models();

	void load_textures();

	struct {
		UniformBufferId environment = 0;
		PerspectiveCamera camera;
	} map_environment;

	struct {
		UniformBufferId environment = 0;
		OrthographicCamera camera = OrthographicCamera(OrthographicCamera::Mode::CLIP_SPACE);
		Framebuffer depth;
	} light_environment;

	struct {
		ShaderId clickable_map;
		ShaderId decoration_map;
		ShaderId model;
		ShaderId model_instanced;
		ShaderId simple_depth;
		ShaderId simple_depth_instanced;
		ShaderId swinging;
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
