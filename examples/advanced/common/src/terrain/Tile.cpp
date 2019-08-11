#include "terrain/Tile.hpp"
#include <cstdlib>

Vertices Tile::get_vertices_translated(float x_pos, float y_pos, const glm::vec3& color_1, const glm::vec3& color_2)
{
	glm::vec3 normal = { 0.0, 1.0f, 0.0f };


	int random_variable = std::rand();
	if (random_variable % 2)
	{
		Vertices vertices =
		{
			/* Vertex pos */  0.0f + x_pos, 0.0f,  0.0f + y_pos, /* Normal */normal.x, normal.y, normal.z,/* Color */ color_1.x, color_1.y, color_1.z,
			/* Vertex pos */  0.0f + x_pos, 0.0f,  1.0f + y_pos, /* Normal */normal.x, normal.y, normal.z,/* Color */ color_2.x, color_2.y, color_2.z,
			/* Vertex pos */  1.0f + x_pos, 0.0f,  1.0f + y_pos, /* Normal */normal.x, normal.y, normal.z,/* Color */ color_2.x, color_2.y, color_2.z,
			/* Vertex pos */  1.0f + x_pos, 0.0f,  0.0f + y_pos, /* Normal */normal.x, normal.y, normal.z,/* Color */ color_2.x, color_2.y, color_2.z,
		};
		return vertices;
	}
	else
	{
		// Same as first, but rotated 90 degress clockwise:

		Vertices vertices =
		{
			/* Vertex pos */  0.0f + x_pos, 0.0f,  1.0f + y_pos, /* Normal */normal.x, normal.y, normal.z,/* Color */ color_1.x, color_1.y, color_1.z,
			/* Vertex pos */  1.0f + x_pos, 0.0f,  1.0f + y_pos, /* Normal */normal.x, normal.y, normal.z,/* Color */ color_2.x, color_2.y, color_2.z,
			/* Vertex pos */  1.0f + x_pos, 0.0f,  0.0f + y_pos, /* Normal */normal.x, normal.y, normal.z,/* Color */ color_2.x, color_2.y, color_2.z,
			/* Vertex pos */  0.0f + x_pos, 0.0f,  0.0f + y_pos, /* Normal */normal.x, normal.y, normal.z,/* Color */ color_2.x, color_2.y, color_2.z,
		};
		return vertices;
	}
}

Attributes Tile::get_attributes()
{
	Attributes attributes =
	{
		{ ShaderAttribute::Type::VEC3, ShaderAttribute::Purpose::VERTEX_POSITION, ShaderAttribute::Buffer::VERTEX},
		{ ShaderAttribute::Type::VEC3, ShaderAttribute::Purpose::NORMAL, ShaderAttribute::Buffer::VERTEX},
		{ ShaderAttribute::Type::VEC3, ShaderAttribute::Purpose::COLOR, ShaderAttribute::Buffer::VERTEX}
	};
	return attributes;
}

Indices Tile::get_indices(std::uint32_t offset)
{
	return
	{
		0 + (offset * 4),
		1 + (offset * 4),
		2 + (offset * 4),
		2 + (offset * 4),
		3 + (offset * 4),
		0 + (offset * 4)
	};
}
