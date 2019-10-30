#pragma once

#include <blue/camera/OrthographicCamera.hpp>
#include <blue/camera/PerspectiveCamera.hpp>
#include <blue/gpu/GpuEntities.hpp>
#include <blue/gui/Button.hpp>

class Resources
{
public:

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

	static Resources& instance();

	static void init();

	static void dispose();

	void load_render_entities();

	void load_environment();

	void load_shaders();

	void load_models();

	void load_textures();

	struct {
		UniformBufferId environment = 0;
		PerspectiveCamera camera = PerspectiveCamera(0, 0);
	} map_environment;

	struct {
		OrthographicCamera camera = OrthographicCamera(OrthographicCamera::Mode::SCREEN_SPACE, 0, 0);
		UniformBufferId environment = 0;
	} gui_environment;

	struct {
		UniformBufferId environment = 0;
		OrthographicCamera camera = OrthographicCamera(OrthographicCamera::Mode::CLIP_SPACE, 0, 0);
		Framebuffer depth;
	} light_environment;

	struct
	{
		RenderEntity selected_tile_highlight;
	} render_entities;

	struct {
		ShaderId clickable_map;
		ShaderId decoration_map;
		ShaderId model;
		ShaderId model_instanced;
		ShaderId simple_depth;
		ShaderId simple_depth_instanced;
		ShaderId swinging;
		ShaderId water;
		ShaderId tile_highlight;
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
		VertexArray square;
	} models;

	struct {
		Texture start;
		Texture start_clicked;
		Texture options;
		Texture options_clicked;
		Texture exit;
		Texture exit_clicked;
	} textures;

private:

	static Resources* _instance;
};
