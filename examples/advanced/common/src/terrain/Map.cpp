#include "terrain/Map.hpp"
#include "blue/Context.hpp"
#include "blue/camera/PerspectiveCamera.hpp"
#include "blue/ShaderUtils.h"
#include "Resources.hpp"

#include <fstream>
#include <istream>
#include "blue/ResourcesPath.h"

#include <glm/gtx/normal.hpp>
#include <Resources.hpp>
#include "glm/geometric.hpp"

void Map::import_from_file(const std::string& filename)
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

	std::size_t offset = 0;
	std::size_t number_of_non_clickable = 0;

	for (std::size_t x = 0; x < CHUNK_DIMENSION; x++)
	{
		for (std::size_t y = 0; y < CHUNK_DIMENSION; y++)
		{
			tiles[x][y].clickable = data.data()[offset];
			offset += sizeof(bool);

			if (!tiles[x][y].clickable)
			{
				number_of_non_clickable++;
			}
		}
	}

	std::size_t number_of_vertices = number_of_non_clickable * Tile::get_vertices_translated(0, 0, { 0, 0,0 }, { 0, 0, 0 }).size();
	decoration_vertices.resize(number_of_vertices);

	for (std::size_t index = 0; index < decoration_vertices.size(); index++)
	{
		const auto float_ptr = reinterpret_cast<float*>(&data[offset]);
		decoration_vertices[index] = *float_ptr;
		offset += sizeof(float);
	}

	for (std::size_t index = 0; index < number_of_non_clickable; index++)
	{
		for (const auto& i : Tile::get_indices(index))
		{
			decoration_indices.push_back(i);
		}
	}

	decoration_tiles = number_of_non_clickable;

	blue::Context::logger().info("Found vertices: {}", number_of_vertices);
}

void Map::export_to_file(const std::string& filename)
{
	std::fstream out(paths::getResourcesPath() + filename, std::fstream::binary | std::fstream::out);

	if (!out.is_open())
	{
		blue::Context::logger().error("Failed to open file: {} for saving map", filename);
		return;
	}

	for (std::size_t x = 0; x < CHUNK_DIMENSION; x++)
	{
		for (std::size_t y = 0; y < CHUNK_DIMENSION; y++)
		{
			out.write(reinterpret_cast<char*>(&tiles[x][y].clickable), sizeof(bool));
		}
	}

	for (std::size_t index = 0; index < decoration_vertices.size(); index++)
	{
		out.write(reinterpret_cast<char*>(&decoration_vertices[index]), sizeof(float));
	}

	out.close();
}


void Map::upload_clickable_vertices()
{
	// randomize color a bit
	std::srand(std::time(nullptr)); // use current time as seed for random generator
	std::unique_lock<std::mutex> lock(tiles_access);
	std::uint32_t tile_index = 0;

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

			for (const auto& vertex : tiles[x][y].get_vertices_translated(static_cast<float>(x), static_cast<float>(y), 0))
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

	blue::Context::logger().info("Clickable vertices: {} Clickable indices: {}", clickable_vertices.size(), clickable_indices.size());
	if (clickable_vertices.empty())
	{
		return;
	}

	auto attributes = Tile::get_attributes();

	auto vertex_array_future = blue::Context::gpu_system().submit(CreateMeshEntity{ clickable_vertices, clickable_indices, attributes, tile_index * 6 });
	VertexArray clickable_vertices_vertex_array = vertex_array_future.get();

	// Submit render command consisting of compiled shader, uploaded mesh and following geometry properties:

	RenderEntity entity;
	entity.position = { 0, 0, 0 };
	entity.shader = Resources::instance().shaders.clickable_map;
	entity.vertex_array = clickable_vertices_vertex_array;
	entity.scale = 1.0f;
	entity.rotation = glm::identity<glm::quat>();
	entity.environment = Resources::instance().map_environment.environment;
	entity.texture = 0;

	clickable_vertices_render_entity = blue::Context::renderer().add(entity);
}

void Map::upload_decoration_vertices()
{
	decoration_vertices.clear();
	decoration_indices.clear();

	std::unique_lock<std::mutex> lock(tiles_access);
	std::uint32_t tile_index = 0;


	for (std::size_t x = 0; x < CHUNK_DIMENSION; x++)
	{
		for (std::size_t y = 0; y < CHUNK_DIMENSION; y++)
		{
			if (tiles[x][y].clickable)
			{
				continue;
			}

			for (const auto& vertex : tiles[x][y].get_vertices_translated(static_cast<float>(x), static_cast<float>(y), 0))
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
	entity.shader = Resources::instance().shaders.decoration_map;
	entity.vertex_array = decoration_vertices_vertex_array;
	entity.scale = 1.0f;
	entity.rotation = glm::identity<glm::quat>();
	entity.environment = Resources::instance().map_environment.environment;
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
	entity.shader = Resources::instance().shaders.decoration_map;
	entity.vertex_array = decoration_vertices_vertex_array;
	entity.scale = 1.0f;
	entity.rotation = glm::identity<glm::quat>();
	//    entity.environment = Resources::instance().light_environment.environment;
	entity.environment = Resources::instance().map_environment.environment;
	entity.texture = Resources::instance().light_environment.depth.texture;
	entity.framebuffer.framebuffer = 0;

	RenderEntity shadow;
	shadow.position = { 0, 0, 0 };
	shadow.shader = Resources::instance().shaders.simple_depth;
	shadow.vertex_array = decoration_vertices_vertex_array;
	shadow.scale = 1.0f;
	shadow.rotation = glm::identity<glm::quat>();
	shadow.environment = Resources::instance().light_environment.environment;
	shadow.texture = 0;
	shadow.framebuffer = Resources::instance().light_environment.depth;

	shadow.id = blue::Context::renderer().add(shadow);
	decoration_vertices_render_entity = blue::Context::renderer().add(entity);
}

void Map::elevate_points(float x, float y, float R, float elevation)
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
				p_1.y += elevation;
				visited[0] = true;
			}

			if (glm::distance(glm::vec3{ p_2.x, 0, p_2.z }, glm::vec3{ x, 0, y }) <= R)
			{
				p_2.y += elevation;
				visited[1] = true;
			}

			if (glm::distance(glm::vec3{ p_3.x, 0, p_3.z }, glm::vec3{ x, 0, y }) <= R)
			{
				p_3.y += elevation;
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
			glm::vec3 p_3 = { decoration_vertices[index + 0 + 0] , decoration_vertices[index + 0 + 1],decoration_vertices[index + 0 + 2] };

			if (!visited[2] && glm::distance(glm::vec3{ p_1.x, 0, p_1.z }, glm::vec3{ x, 0, y }) <= R)
			{
				p_1.y += elevation;
			}

			if (glm::distance(glm::vec3{ p_2.x, 0, p_2.z }, glm::vec3{ x, 0, y }) <= R)
			{
				p_2.y += elevation;
			}

			if (!visited[0] && glm::distance(glm::vec3{ p_3.x, 0, p_3.z }, glm::vec3{ x, 0, y }) <= R)
			{
				p_3.y += elevation;
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

void Map::color_tile(std::uint16_t x, std::uint16_t y, InterpolationDirection interpolation, const glm::vec3& color_1, const glm::vec3& color_2)
{
	{
		Tile& center = tiles[x][y];
		center.interpolation = interpolation;
		center.color_1 = color_1;
		center.color_2 = color_2;
	}

	for (std::uint16_t tile_x = 0; tile_x < CHUNK_DIMENSION; tile_x++)
	{
		for (std::uint16_t tile_y = 0; tile_y < CHUNK_DIMENSION; tile_y++)
		{
			Tile& current = tiles[tile_x][tile_y];
			if (current.interpolation == InterpolationDirection::FULL) continue;

			Tile* center_left = nullptr;
			Tile* center_right = nullptr;
			Tile* center_up = nullptr;
			Tile* center_down = nullptr;
			Tile* corner_left_down = nullptr;
			Tile* corner_left_up = nullptr;
			Tile* corner_right_down = nullptr;
			Tile* corner_right_up = nullptr;

			// left / right
			if (tile_x - 1 > 0) center_left = &tiles[tile_x - 1][tile_y];
			if (tile_x + 1 < CHUNK_DIMENSION) center_right = &tiles[tile_x + 1][tile_y];
			// up / down
			if (tile_y - 1 > 0) center_up = &tiles[tile_x][tile_y - 1];
			if (tile_y + 1 < CHUNK_DIMENSION) center_down = &tiles[tile_x][tile_y + 1];
			// left corners
			if (tile_x - 1 > 0 && tile_y - 1 > 0) corner_left_down = &tiles[tile_x - 1][tile_y - 1];
			if (tile_x - 1 > 0 && tile_y + 1 < CHUNK_DIMENSION) corner_left_up = &tiles[tile_x - 1][tile_y + 1];
			// right corners
			if (tile_x + 1 < CHUNK_DIMENSION && tile_y - 1 > 0) corner_right_down = &tiles[tile_x + 1][tile_y - 1];
			if (tile_x + 1 < CHUNK_DIMENSION && tile_y + 1 < CHUNK_DIMENSION) corner_right_up = &tiles[tile_x + 1][tile_y + 1];

			// left / right test
			if (center_left && center_left->interpolation == InterpolationDirection::FULL)
			{
				current.interpolation = InterpolationDirection::HORIZONTAL_LEFT_RIGHT;
				current.color_1 = center_left->color_1;
				current.color_2 = center_left->color_2;
			}
			if (center_right && center_right->interpolation == InterpolationDirection::FULL)
			{
				current.interpolation = InterpolationDirection::HORIZONTAL_LEFT_RIGHT;
				current.color_1 = center_right->color_2;
				current.color_2 = center_right->color_1;
			}
			// up / down test
			if (center_up && center_up->interpolation == InterpolationDirection::FULL)
			{
				current.interpolation = InterpolationDirection::VERTICAL_UP_DOWN;
				current.color_1 = center_up->color_1;
				current.color_2 = center_up->color_2;
			}
			if (center_down && center_down->interpolation == InterpolationDirection::FULL)
			{
				current.interpolation = InterpolationDirection::VERTICAL_UP_DOWN;
				current.color_2 = center_down->color_1;
				current.color_1 = center_down->color_2;
			}

			// right corner's test
			if (corner_right_up && center_right && center_up)
			{
				if (corner_right_up->interpolation == InterpolationDirection::FULL &&
					center_right->interpolation == InterpolationDirection::FULL &&
					center_down->interpolation == InterpolationDirection::FULL)
				{
					current.interpolation = InterpolationDirection::CORNER_RIGHT_DOWN;
					current.color_2 = corner_right_up->color_2;
					current.color_1 = corner_right_up->color_1;
				}
			}
			if (corner_right_down && center_right && center_down)
			{
				if (corner_right_down->interpolation == InterpolationDirection::FULL &&
					center_right->interpolation == InterpolationDirection::FULL &&
					center_up->interpolation == InterpolationDirection::FULL)
				{
					current.interpolation = InterpolationDirection::CORNER_LEFT_DOWN;
					current.color_1 = corner_right_down->color_2;
					current.color_2 = corner_right_down->color_1;
				}
			}
			// left corner's test
			if (corner_left_up && center_left && center_up)
			{
				if (corner_left_up->interpolation == InterpolationDirection::FULL &&
					center_left->interpolation == InterpolationDirection::FULL &&
					center_down->interpolation == InterpolationDirection::FULL)
				{
					current.interpolation = InterpolationDirection::CORNER_RIGHT_UP;
					current.color_1 = corner_left_up->color_2;
					current.color_2 = corner_left_up->color_1;
				}
			}
			if (corner_left_down && center_left && center_up)
			{
				if (corner_left_down->interpolation == InterpolationDirection::FULL &&
					center_left->interpolation == InterpolationDirection::FULL &&
					center_up->interpolation == InterpolationDirection::FULL)
				{
					current.interpolation = InterpolationDirection::CORNER_LEFT_UP;
					current.color_2 = corner_left_down->color_2;
					current.color_1 = corner_left_down->color_1;
				}
			}
			
			// left corner's corner's test
			if (corner_left_down && center_left && center_up)
			{
				if (corner_left_down->interpolation == InterpolationDirection::FULL &&
					center_left->interpolation == InterpolationDirection::VERTICAL_UP_DOWN &&
					center_up->interpolation == InterpolationDirection::HORIZONTAL_LEFT_RIGHT)
				{
					current.interpolation = InterpolationDirection::CORNER_RIGHT_DOWN;
					current.color_1 = corner_left_down->color_2;
					current.color_2 = corner_left_down->color_1;
				}
			}
			if (corner_left_up && center_left && center_down)
			{
				if (corner_left_up->interpolation == InterpolationDirection::FULL &&
					center_left->interpolation == InterpolationDirection::VERTICAL_UP_DOWN &&
					center_down->interpolation == InterpolationDirection::HORIZONTAL_LEFT_RIGHT)
				{
					current.interpolation = InterpolationDirection::CORNER_LEFT_DOWN;
					current.color_1 = corner_left_up->color_1;
					current.color_2 = corner_left_up->color_2;
				}
			}
			// right corner's corner's test
			if (corner_right_down && center_right && center_up)
			{
				if (corner_right_down->interpolation == InterpolationDirection::FULL &&
					center_right->interpolation == InterpolationDirection::VERTICAL_UP_DOWN &&
					center_up->interpolation == InterpolationDirection::HORIZONTAL_LEFT_RIGHT)
				{
					current.interpolation = InterpolationDirection::CORNER_RIGHT_UP;
					current.color_1 = corner_right_down->color_1;
					current.color_2 = corner_right_down->color_2;
				}
			}
			if (corner_right_up && center_right && center_down)
			{
				if (corner_right_up->interpolation == InterpolationDirection::FULL &&
					center_right->interpolation == InterpolationDirection::VERTICAL_UP_DOWN &&
					center_down->interpolation == InterpolationDirection::HORIZONTAL_LEFT_RIGHT)
				{
					current.interpolation = InterpolationDirection::CORNER_LEFT_UP;
					current.color_1 = corner_right_up->color_2;
					current.color_2 = corner_right_up->color_1;
				}
			}
			// right corner's corner's corner's test
			if (corner_right_down && center_right && center_up)
			{
				if (corner_right_down->interpolation == InterpolationDirection::FULL &&
					(center_right->interpolation == InterpolationDirection::CORNER_LEFT_DOWN ||
						center_right->interpolation == InterpolationDirection::CORNER_LEFT_UP ||
						center_right->interpolation == InterpolationDirection::CORNER_RIGHT_DOWN || 
						center_right->interpolation == InterpolationDirection::CORNER_RIGHT_UP) &&
						(center_up->interpolation == InterpolationDirection::CORNER_LEFT_DOWN ||
							center_up->interpolation == InterpolationDirection::CORNER_LEFT_UP ||
							center_up->interpolation == InterpolationDirection::CORNER_RIGHT_DOWN ||
							center_up->interpolation == InterpolationDirection::CORNER_RIGHT_UP))
				{
					current.interpolation = InterpolationDirection::CORNER_RIGHT_UP;
					current.color_1 = corner_right_down->color_1;
					current.color_2 = corner_right_down->color_2;
				}
			}
			if (corner_right_up && center_right && center_down)
			{
				if (corner_right_up->interpolation == InterpolationDirection::FULL &&
					(center_right->interpolation == InterpolationDirection::CORNER_LEFT_DOWN ||
						center_right->interpolation == InterpolationDirection::CORNER_LEFT_UP ||
						center_right->interpolation == InterpolationDirection::CORNER_RIGHT_DOWN || 
						center_right->interpolation == InterpolationDirection::CORNER_RIGHT_UP) &&
						(center_down->interpolation == InterpolationDirection::CORNER_LEFT_DOWN ||
							center_down->interpolation == InterpolationDirection::CORNER_LEFT_UP ||
							center_down->interpolation == InterpolationDirection::CORNER_RIGHT_DOWN ||
							center_down->interpolation == InterpolationDirection::CORNER_RIGHT_UP))
				{
					current.interpolation = InterpolationDirection::CORNER_LEFT_UP;
					current.color_1 = corner_right_up->color_1;
					current.color_2 = corner_right_up->color_2;
				}
			}
			// left corner's corner's corner's test
			if (corner_left_down && center_left && center_up)
			{
				if (corner_left_down->interpolation == InterpolationDirection::FULL &&
					(center_left->interpolation == InterpolationDirection::CORNER_LEFT_DOWN ||
						center_left->interpolation == InterpolationDirection::CORNER_LEFT_UP ||
						center_left->interpolation == InterpolationDirection::CORNER_RIGHT_DOWN ||
						center_left->interpolation == InterpolationDirection::CORNER_RIGHT_UP) &&
						(center_up->interpolation == InterpolationDirection::CORNER_LEFT_DOWN ||
							center_up->interpolation == InterpolationDirection::CORNER_LEFT_UP ||
							center_up->interpolation == InterpolationDirection::CORNER_RIGHT_DOWN ||
							center_up->interpolation == InterpolationDirection::CORNER_RIGHT_UP))
				{
					current.interpolation = InterpolationDirection::CORNER_RIGHT_DOWN;
					current.color_1 = corner_left_down->color_1;
					current.color_2 = corner_left_down->color_2;
				}
			}
			if (corner_left_up && center_left && center_down)
			{
				if (corner_left_up->interpolation == InterpolationDirection::FULL &&
					(center_left->interpolation == InterpolationDirection::CORNER_LEFT_DOWN ||
						center_left->interpolation == InterpolationDirection::CORNER_LEFT_UP ||
						center_left->interpolation == InterpolationDirection::CORNER_RIGHT_DOWN ||
						center_left->interpolation == InterpolationDirection::CORNER_RIGHT_UP) &&
						(center_down->interpolation == InterpolationDirection::CORNER_LEFT_DOWN ||
							center_down->interpolation == InterpolationDirection::CORNER_LEFT_UP ||
							center_down->interpolation == InterpolationDirection::CORNER_RIGHT_DOWN ||
							center_down->interpolation == InterpolationDirection::CORNER_RIGHT_UP))
				{
					current.interpolation = InterpolationDirection::CORNER_LEFT_DOWN;
					current.color_1 = corner_left_up->color_1;
					current.color_2 = corner_left_up->color_2;
				}
			}
		}
	}
}

std::pair<glm::vec3, glm::vec3> Map::get_color(std::uint16_t x, std::uint16_t y)
{
	Tile& tile = tiles[x][y];
	return std::make_pair(tile.color_1, tile.color_2);
}
