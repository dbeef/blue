#pragma once

#include "blue/Renderer.h"

class Tile
{
public:

	static Vertices get_vertices_translated(float x_pos, float y_pos);
	static Indices get_indices(std::uint32_t offset = 0);
	static Attributes get_attributes();

	bool clickable = true;

};
