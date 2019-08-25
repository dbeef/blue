#include "terrain/Water.hpp"
#include "blue/Context.hpp"
#include "Resources.hpp"

#include <algorithm>

void Water::import_from_file(const std::string& filename)
{
}

void Water::export_to_file(const std::string& filename)
{
}

void Water::create_water(const Map& map, float water_level_y)
{
	bool tile_has_water[Map::CHUNK_DIMENSION][Map::CHUNK_DIMENSION] = { false };
	const glm::vec3 water_color = {0.1f, 0.3f, 0.8f};
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

		if (y_1 < water_level_y || y_2 < water_level_y)
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

			for (const auto& vertex : Tile::get_vertices_translated(index_x, water_level_y, index_z, water_color))
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
	entity.shader = Resources::instance().shaders.decoration_map;
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
