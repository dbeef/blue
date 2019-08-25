#pragma once

#include "terrain/Map.hpp"

#include <string>

class Water
{

public:

	void import_from_file(const std::string& filename);
	void export_to_file(const std::string& filename);

	void set_water_level_y(float);
	void create_water(const Map& map);
	void upload_water();

private:

	bool _has_water = false;
	float _water_level_y = 0;

	bool tile_has_water[Map::CHUNK_DIMENSION][Map::CHUNK_DIMENSION] = {false};

	Vertices _vertices;
	Indices _indices;
	RenderEntityId _render_entity;
};