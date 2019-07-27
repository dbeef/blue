#pragma once

class Resources
{
public:

	static Resources& instance();

	static void init();

	static void dispose();

	void load_shaders();

	void load_models();

	void load_textures();

private:

	static Resources* _instance;
};
