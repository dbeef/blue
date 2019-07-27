#include "terrain/Tile.hpp"

Vertices Tile::get_vertices_translated(float x_pos, float y_pos)
{
	Vertices vertices =
	{
		/* Vertex pos */ -1.0f + x_pos, 0.0f, -1.0f + y_pos, /* Color */ 0.1f, 0.7f, 0.25f,
		/* Vertex pos */ -1.0f + x_pos, 0.0f,  1.0f + y_pos, /* Color */ 0.1f, 0.7f, 0.25f,
		/* Vertex pos */  1.0f + x_pos, 0.0f,  1.0f + y_pos, /* Color */ 0.1f, 0.7f, 0.25f,
		/* Vertex pos */  1.0f + x_pos, 0.0f, -1.0f + y_pos, /* Color */ 0.1f, 0.7f, 0.25f,
	};

	return vertices;
}

Attributes Tile::get_attributes()
{
	Attributes attributes =
	{
		{ ShaderAttribute::Type::VEC3, ShaderAttribute::Purpose::VERTEX_POSITION},
		{ ShaderAttribute::Type::VEC3, ShaderAttribute::Purpose::COLOR}
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
