#pragma once

#include "blue/Renderer.h"

enum class InterpolationDirection
{
	HORIZONTAL_LEFT_RIGHT = 0,
	VERTICAL_UP_DOWN,
	CORNER_LEFT_UP,
	CORNER_RIGHT_DOWN,
	CORNER_LEFT_DOWN,
	CORNER_RIGHT_UP,
	FULL,
	INVALID
};

class Tile
{
public:

	static Vertices get_vertices_translated(float x_pos, float y_pos, const glm::vec3& color_1, const glm::vec3& color_2);
	static Vertices get_vertices_translated(float x_pos, float y_pos, float z_pos, const glm::vec3& color);
	Vertices get_vertices_translated(float x_pos, float y_pos, float z_pos);

	static Indices get_indices(std::uint32_t offset = 0);
	static Attributes get_attributes();

	bool clickable = false;
	InterpolationDirection interpolation = InterpolationDirection::INVALID;
	glm::vec3 color_1 = { 78.0f / 255, 192.0f / 255, 29.0f / 255, };
	glm::vec3 color_2 = { 78.0f / 255, 192.0f / 255, 29.0f / 255, };
};
