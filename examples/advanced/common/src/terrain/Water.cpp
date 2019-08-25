#include "terrain/Water.hpp"
#include "blue/Context.hpp"
#include "Resources.hpp"
#include "blue/ResourcesPath.h"

#include <algorithm>
#include <fstream>

void Water::import_from_file(const std::string& filename)
{
	auto path_extended = paths::getResourcesPath() + filename;

	std::vector<char> data;

	SDL_RWops* file = SDL_RWFromFile(path_extended.c_str(), "rb");
	if (file != NULL) {
		auto size = file->size(file);
		//Initialize data
		data.resize(size);
		auto read = file->read(file, &data[0], size, 1);
		//Close file handler
		SDL_RWclose(file);
	}
	else
	{
		blue::Context::logger().error("Error: Unable to open map file. SDL Error: {}", SDL_GetError());
		return;
	}

	std::uint64_t offset = 0;
	_has_water = *reinterpret_cast<bool*>(&data[offset]);
	offset += sizeof(bool);
	_water_level_y = *reinterpret_cast<float*>(&data[offset]);
	offset += sizeof(float);

	blue::Context::logger().info("Has water: {}, water level: {}", _has_water, _water_level_y);
}

void Water::export_to_file(const std::string& filename)
{
	std::fstream out(paths::getResourcesPath() + filename, std::fstream::binary | std::fstream::out);

	if (!out.is_open())
	{
		blue::Context::logger().error("Failed to open file: {} for saving map", filename);
		return;
	}

	out.write(reinterpret_cast<char*>(&_has_water), sizeof(bool));
	out.write(reinterpret_cast<char*>(&_water_level_y), sizeof(float));

	out.close();
}

void Water::set_water_level_y(float water_level_y)
{
	_water_level_y = water_level_y;
}

void Water::create_water(const Map& map)
{
	_has_water = true;

	const glm::vec3 water_color = { 0.20f, 0.35f, 1.0f };
	_vertices.clear();
	_indices.clear();

	// Iterate over every square quering for its height:
	for (std::size_t index = 0; index < map.decoration_vertices.size(); index += (2 * 9))
	{
		// First triangle

		VertexType x_1 = map.decoration_vertices[index + 0];
		VertexType y_1 = map.decoration_vertices[index + 1];
		VertexType z_1 = map.decoration_vertices[index + 2];

		// Second triangle

		VertexType x_2 = map.decoration_vertices[index + 0 + 9];
		VertexType y_2 = map.decoration_vertices[index + 1 + 9];
		VertexType z_2 = map.decoration_vertices[index + 2 + 9];

		if (y_1 < _water_level_y || y_2 < _water_level_y)
		{
			// Calculate tile that this vertex is from;
			// Use vertex's X and Z values, since they never get edited:
			
			auto index_x_1 = static_cast<int>(x_1);
			auto index_z_1 = static_cast<int>(z_1);

			auto index_x_2 = static_cast<int>(x_1);
			auto index_z_2 = static_cast<int>(z_1);

			tile_has_water[index_x_1][index_z_1] = true;
			tile_has_water[index_x_2][index_z_2] = true;
		
			if (index_x_1 + 1< Map::CHUNK_DIMENSION && index_x_2 + 1 < Map::CHUNK_DIMENSION)
			{
				tile_has_water[index_x_1 + 1][index_z_1] = true;
				tile_has_water[index_x_2 + 1][index_z_2] = true;
			}
			if (index_x_1 > 0 && index_x_2 > 0)
			{
				tile_has_water[index_x_1 - 1][index_z_1] = true;
				tile_has_water[index_x_2 - 1][index_z_2] = true;
			}
			
			if (index_z_1 + 1< Map::CHUNK_DIMENSION && index_z_2 + 1 < Map::CHUNK_DIMENSION)
			{
				tile_has_water[index_x_1][index_z_1 + 1] = true;
				tile_has_water[index_x_2][index_z_2 + 1] = true;
			}
			if (index_z_1 > 0 && index_z_2 > 0)
			{
				tile_has_water[index_x_1][index_z_1 - 1] = true;
				tile_has_water[index_x_2][index_z_2 - 1] = true;
			}

		}
	}

	std::uint32_t tile_index = 0;
	for (std::size_t index_x = 0; index_x < Map::CHUNK_DIMENSION; index_x++)
	{
		for (std::size_t index_z = 0; index_z < Map::CHUNK_DIMENSION; index_z++)
		{
			if (!tile_has_water[index_x][index_z]) continue;

			for (const auto& vertex : Tile::get_vertices_translated(index_x, _water_level_y, index_z, water_color))
			{
				_vertices.push_back(vertex);
			}
			for (const auto& index : Tile::get_indices(tile_index))
			{
				_indices.push_back(index);
			}
			tile_index++;
		}
	}

	auto attributes = Tile::get_attributes();

	auto vertex_array_future = blue::Context::gpu_system().submit(CreateMeshEntity{ _vertices, _indices, attributes, tile_index * 6 });
	VertexArray clickable_vertices_vertex_array = vertex_array_future.get();

	// Submit render command consisting of compiled shader, uploaded mesh and following geometry properties:

	RenderEntity entity;
	entity.position = { 0, 0, 0 };
	entity.shader = Resources::instance().shaders.water;
	entity.vertex_array = clickable_vertices_vertex_array;
	entity.scale = 1.0f;
	entity.rotation = glm::identity<glm::quat>();
	entity.environment = Resources::instance().map_environment.environment;
	entity.texture = 0;

	_render_entity = blue::Context::renderer().add(entity);
}

void Water::upload_water()
{
}
