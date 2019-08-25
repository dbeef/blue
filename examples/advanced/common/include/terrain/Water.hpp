#pragma once

#include "terrain/Map.hpp"

#include <string>

class Water
{

public:

	void import_from_file(const std::string& filename);
	void export_to_file(const std::string& filename);

	void create_water(const Map& map, float water_level_y);
	void upload_water();

private:

	Vertices _vertices;
	Indices _indices;
	RenderEntityId _render_entity;
};