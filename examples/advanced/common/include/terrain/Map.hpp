#pragma once

#include "blue/Renderer.h"
#include "blue/camera/PerspectiveCamera.hpp"
#include "terrain/Tile.hpp"
#include <mutex>

class Map
{
	friend class MapIntersectionJob;
	friend class Water;

public:

	static const int CHUNK_DIMENSION = 64;

	void upload_clickable_vertices();
	void upload_decoration_vertices();

	void toggle_tile(std::uint16_t x, std::uint16_t y);
	void dispose_current_map_on_gpu();
	void dispose_current_decoration_on_gpus();
	void upload_decoration();
	void upload_clickable();

	void color_clickable_points(float x, float y, float R, const glm::vec3& color);
	void color_decoration_points(float x, float y, float R, const glm::vec3& color);

	void elevate_clickable_points(float x, float y, float R, float elevation);
	void elevate_decoration_points(float x, float y, float R, float elevation);

	void shuffle_color_points(float x, float y, float R);

	void import_from_file(const std::string& filename);
	void export_to_file(const std::string& filename);

	void set_tile_occupant(UnitType occupant, std::uint16_t tile_x, std::uint16_t tile_y);
	UnitType get_tile_occupant(std::uint16_t tile_x, std::uint16_t tile_y);

private:

	void color_points(float x, float y, float R, const glm::vec3& color, Vertices& vertices);
	void elevate_points(float x, float y, float R, float elevation, Vertices& vertices);

	std::uint32_t decoration_tiles = 0;
	std::uint32_t clickable_tiles = 0;

	Vertices decoration_vertices;
	Indices decoration_indices;

	Vertices clickable_vertices;
	Indices clickable_indices;

	std::mutex tiles_access;
	Tile tiles[CHUNK_DIMENSION][CHUNK_DIMENSION];

	RenderEntityId clickable_vertices_render_entity;
	RenderEntityId decoration_vertices_render_entity;
};
