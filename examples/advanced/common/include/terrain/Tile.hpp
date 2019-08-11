#pragma once

#include "blue/Renderer.h"

class Tile
{
public:

	static Vertices get_vertices_translated(float x_pos, float y_pos, const glm::vec3& color_1, const glm::vec3& color_2);
	static Indices get_indices(std::uint32_t offset = 0);
	static Attributes get_attributes();

	bool clickable = false;

};
