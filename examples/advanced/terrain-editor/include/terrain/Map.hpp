#pragma once

#include "blue/Renderer.h"
#include "blue/camera/PerspectiveCamera.hpp"
#include "terrain/Tile.hpp"
#include <mutex>

class Map
{
	friend class MapIntersectionJob;

public:

	static const int CHUNK_DIMENSION = 32;

	void upload_clickable_vertices();
	void upload_decoration_vertices();

	void toggle_tile(std::uint16_t x, std::uint16_t y);
	void dispose_current_map_on_gpu();

private:

	
	std::mutex tiles_access;
	Tile tiles[CHUNK_DIMENSION][CHUNK_DIMENSION];

	RenderEntityId clickable_vertices_render_entity;
	RenderEntityId decoration_vertices_render_entity;
};
