#include "terrain/Map.hpp"
#include "blue/Context.hpp"
#include "blue/camera/PerspectiveCamera.hpp"
#include "blue/ShaderUtils.h"
#include "Resources.hpp"
#include "Application.hpp"

#include <glm/gtx/normal.hpp>
#include "glm/geometric.hpp"

void Map::upload_clickable_vertices()
{
	std::unique_lock<std::mutex> lock(tiles_access);
	std::uint32_t tile_index = 0;
	const glm::vec3 clickable_color = { 0.2f, 0.8f, 0.5f };

	Vertices clickable_vertices;
	Indices clickable_indices;

	for (std::size_t x = 0; x < CHUNK_DIMENSION; x++)
	{
		for (std::size_t y = 0; y < CHUNK_DIMENSION; y++)
		{
			if (!tiles[x][y].clickable)
			{
				continue;
			}
			for (const auto& vertex : Tile::get_vertices_translated(static_cast<float>(x), static_cast<float>(y), clickable_color))
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

	auto vertex_array_future = blue::Context::gpu_system().submit(CreateMeshEntity{ clickable_vertices, clickable_indices, attributes, tile_index * 6 });
	vertex_array_future.wait();

	VertexArray clickable_vertices_vertex_array = vertex_array_future.get();

	// Submit render command consisting of compiled shader, uploaded mesh and following geometry properties:

	RenderEntity entity;
	entity.position = { 0, 0, 0 };
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
	decoration_vertices.clear();
	decoration_indices.clear();

	std::unique_lock<std::mutex> lock(tiles_access);
	std::uint32_t tile_index = 0;
	const glm::vec3 decoration_color = { 0.5f, 0.8f, 0.5f };

	for (std::size_t x = 0; x < CHUNK_DIMENSION; x++)
	{
		for (std::size_t y = 0; y < CHUNK_DIMENSION; y++)
		{
			if (tiles[x][y].clickable)
			{
				continue;
			}
			for (const auto& vertex : Tile::get_vertices_translated(static_cast<float>(x), static_cast<float>(y), decoration_color))
			{
				decoration_vertices.push_back(vertex);
			}
			for (const auto& index : Tile::get_indices(tile_index))
			{
				decoration_indices.push_back(index);
			}
			tile_index++;
		}
	}

	auto attributes = Tile::get_attributes();
	auto vertex_array_future = blue::Context::gpu_system().submit(CreateMeshEntity{ decoration_vertices, decoration_indices, attributes, tile_index * 6 });
	vertex_array_future.wait();
	VertexArray decoration_vertices_vertex_array = vertex_array_future.get();

	decoration_tiles = tile_index;

	// Submit render command consisting of compiled shader, uploaded mesh and following geometry properties:

	RenderEntity entity;
	entity.position = { 0, 0, 0 };
	entity.shader = Resources::instance().shaders.decoration_map_shader;
	entity.vertex_array = decoration_vertices_vertex_array;
	entity.scale = 1.0f;
	entity.rotation = glm::identity<glm::quat>();
	entity.environment = Application::instance().map_environment.environment;
	entity.texture = 0;

	decoration_vertices_render_entity = blue::Context::renderer().add(entity);
}

void Map::toggle_tile(std::uint16_t x, std::uint16_t y)
{
	std::unique_lock<std::mutex> lock(tiles_access);
	tiles[x][y].clickable = !tiles[x][y].clickable;
}

void Map::dispose_current_map_on_gpu()
{
	blue::Context::renderer().remove_render_entity(clickable_vertices_render_entity);
	blue::Context::renderer().remove_render_entity(decoration_vertices_render_entity);
	clickable_vertices_render_entity = 0;
	decoration_vertices_render_entity = 0;
}

void Map::dispose_current_decoration_on_gpus()
{
	blue::Context::renderer().remove_render_entity(decoration_vertices_render_entity);
	decoration_vertices_render_entity = 0;
}

void Map::upload_decoration()
{
	auto attributes = Tile::get_attributes();
	auto vertex_array_future = blue::Context::gpu_system().submit(CreateMeshEntity{ decoration_vertices, decoration_indices, attributes, decoration_tiles * 6 });
	vertex_array_future.wait();
	VertexArray decoration_vertices_vertex_array = vertex_array_future.get();

	// Submit render command consisting of compiled shader, uploaded mesh and following geometry properties:

	RenderEntity entity;
	entity.position = { 0, 0, 0 };
	entity.shader = Resources::instance().shaders.decoration_map_shader;
	entity.vertex_array = decoration_vertices_vertex_array;
	entity.scale = 1.0f;
	entity.rotation = glm::identity<glm::quat>();
	entity.environment = Application::instance().map_environment.environment;
	entity.texture = 0;

	decoration_vertices_render_entity = blue::Context::renderer().add(entity);
}

void Map::ascend_points(float x, float y, float R)
{
	// Iterate over every 2 triangles
	for (std::size_t index = 0; index < decoration_vertices.size(); index += (9 * 4))
	{
		// TODO: Check if this vertex is adjacent to clickable tile, if so, ignore it
		// to avoid voids in terrain. 

		bool visited[6] = { false };

		// First triangle indexes:  0, 1, 2
		{
			glm::vec3 p_1 = { decoration_vertices[index + 0 + 0] , decoration_vertices[index + 0 + 1],decoration_vertices[index + 0 + 2] };
			glm::vec3 p_2 = { decoration_vertices[index + 9 + 0] , decoration_vertices[index + 9 + 1],decoration_vertices[index + 9 + 2] };
			glm::vec3 p_3 = { decoration_vertices[index + 18 + 0] , decoration_vertices[index + 18 + 1],decoration_vertices[index + 18 + 2] };

			if (glm::distance(glm::vec3{ p_1.x, 0, p_1.z }, glm::vec3{ x, 0, y }) <= R)
			{
				p_1.y += 0.25f;
				visited[0] = true;
			}
			
			if (glm::distance(glm::vec3{ p_2.x, 0, p_2.z }, glm::vec3{ x, 0, y }) <= R)
			{
				p_2.y += 0.25f;
				visited[1] = true;
			}
			
			if (glm::distance(glm::vec3{ p_3.x, 0, p_3.z }, glm::vec3{ x, 0, y }) <= R)
			{
				p_3.y += 0.25f;
				visited[2] = true;
			}

			const auto normal = glm::triangleNormal(p_1, p_2, p_3);

			decoration_vertices[index + 0 + 0] = p_1.x;
			decoration_vertices[index + 0 + 1] = p_1.y;
			decoration_vertices[index + 0 + 2] = p_1.z;

			decoration_vertices[index + 0 + 3] = normal.x;
			decoration_vertices[index + 0 + 4] = normal.y;
			decoration_vertices[index + 0 + 5] = normal.z;

			decoration_vertices[index + 9 + 0] = p_2.x;
			decoration_vertices[index + 9 + 1] = p_2.y;
			decoration_vertices[index + 9 + 2] = p_2.z;

			decoration_vertices[index + 9 + 3] = normal.x;
			decoration_vertices[index + 9 + 4] = normal.y;
			decoration_vertices[index + 9 + 5] = normal.z;

			decoration_vertices[index + 18 + 0] = p_3.x;
			decoration_vertices[index + 18 + 1] = p_3.y;
			decoration_vertices[index + 18 + 2] = p_3.z;

			decoration_vertices[index + 18 + 3] = normal.x;
			decoration_vertices[index + 18 + 4] = normal.y;
			decoration_vertices[index + 18 + 5] = normal.z;
		}
		// Second traingle indexes: 2, 3, 0
		{
			glm::vec3 p_1 = { decoration_vertices[index + 18 + 0] , decoration_vertices[index + 18 + 1],decoration_vertices[index + 18 + 2] };
			glm::vec3 p_2 = { decoration_vertices[index + 27 + 0] , decoration_vertices[index + 27 + 1],decoration_vertices[index + 27 + 2] };
			glm::vec3 p_3 = { decoration_vertices[index + 0  + 0] , decoration_vertices[index + 0  + 1],decoration_vertices[index + 0  + 2] };

			if (!visited[2] && glm::distance(glm::vec3{ p_1.x, 0, p_1.z }, glm::vec3{ x, 0, y }) <= R)
			{
				p_1.y += 0.25f;
			}

			if (glm::distance(glm::vec3{ p_2.x, 0, p_2.z }, glm::vec3{ x, 0, y }) <= R)
			{
				p_2.y += 0.25f;
			}

			if (!visited[0] && glm::distance(glm::vec3{ p_3.x, 0, p_3.z }, glm::vec3{ x, 0, y }) <= R)
			{
				p_3.y += 0.25f;
			}

			const auto normal = glm::triangleNormal(p_1, p_2, p_3);

			decoration_vertices[index + 18 + 0] = p_1.x;
			decoration_vertices[index + 18 + 1] = p_1.y;
			decoration_vertices[index + 18 + 2] = p_1.z;

			decoration_vertices[index + 18 + 3] = normal.x;
			decoration_vertices[index + 18 + 4] = normal.y;
			decoration_vertices[index + 18 + 5] = normal.z;
			
			decoration_vertices[index + 27 + 0] = p_2.x;
			decoration_vertices[index + 27 + 1] = p_2.y;
			decoration_vertices[index + 27 + 2] = p_2.z;

			decoration_vertices[index + 27 + 3] = normal.x;
			decoration_vertices[index + 27 + 4] = normal.y;
			decoration_vertices[index + 27 + 5] = normal.z;

			decoration_vertices[index + 0 + 0] = p_3.x;
			decoration_vertices[index + 0 + 1] = p_3.y;
			decoration_vertices[index + 0 + 2] = p_3.z;

			decoration_vertices[index + 0 + 3] = normal.x;
			decoration_vertices[index + 0 + 4] = normal.y;
			decoration_vertices[index + 0 + 5] = normal.z;
		}

	}
}
