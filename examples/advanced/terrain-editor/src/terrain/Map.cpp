#include "terrain/Map.hpp"
#include "blue/Context.hpp"
#include "blue/camera/PerspectiveCamera.hpp"
#include "blue/ShaderUtils.h"
#include "Resources.hpp"
#include "Application.hpp"

void Map::upload_clickable_vertices()
{
	std::uint32_t tile_index = 0;

	for (std::size_t x = 0; x < CHUNK_DIMENSION; x++)
	{
		for (std::size_t y = 0; y < CHUNK_DIMENSION; y++)
		{
			if (!tiles[x][y].clickable) continue;
			for (const auto& vertex : Tile::get_vertices_translated(static_cast<float>(x), static_cast<float>(y)))
			{
				clickable_vertices.push_back(vertex);
			}
			for (const auto& index : Tile::get_indices(tile_index))
			{
				clickable_indices.push_back(index);
			}
			tile_index++;
		}
	}

	auto attributes = Tile::get_attributes();

	auto vertex_array_future = blue::Context::gpu_system().submit(CreateMeshEntity{ clickable_vertices, clickable_indices, attributes, tile_index * 6});
	vertex_array_future.wait();
	clickable_vertices_vertex_array = vertex_array_future.get();

	// Submit render command consisting of compiled shader, uploaded mesh and following geometry properties:

	RenderEntity entity; 
	entity.position = { 0, 0, 0};
	entity.shader = Resources::instance().shaders.clickable_map_shader;
	entity.vertex_array = clickable_vertices_vertex_array;
	entity.scale = 1.0f;
	entity.rotation = glm::identity<glm::quat>();
	entity.environment = Application::instance().map_environment.environment;
	entity.texture = 0;

	clickable_vertices_render_entity = blue::Context::renderer().add(entity);
}

void Map::upload_decoration_vertices()
{
}
