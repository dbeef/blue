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

private:

	std::mutex tiles_access;
	Tile tiles[CHUNK_DIMENSION][CHUNK_DIMENSION];

	Vertices clickable_vertices;
	Indices clickable_indices;
	Vertices decoration_vertices;
	Indices decoration_indices;

	VertexArray clickable_vertices_vertex_array;
	VertexArray decoration_vertices_vertex_array;
	
	RenderEntityId clickable_vertices_render_entity;
	RenderEntityId decoration_vertices_render_entity;
};
