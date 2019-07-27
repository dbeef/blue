#pragma once

#include "blue/Renderer.h"
#include "blue/camera/PerspectiveCamera.hpp"
#include "terrain/Tile.hpp"
#include <mutex>

static const int CHUNK_DIMENSION = 32;

class Map
{
public:

	void upload_clickable_vertices();
	void upload_decoration_vertices();

private:

	std::mutex tiles_access;
	Tile tiles[CHUNK_DIMENSION][CHUNK_DIMENSION];

	Vertices clickable_vertices;
	Indices clickable_indices;
	Vertices decoration_vertices;
	Indices decoration_indices;

	UniformBufferId environment = 0;
	PerspectiveCamera camera;

	VertexArray clickable_vertices_id;
	VertexArray decoration_vertices_id;
};
