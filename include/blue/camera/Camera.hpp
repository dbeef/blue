#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "glad/glad.h"

using CameraId = std::uint32_t;

class Camera
{
public:

	Camera(std::uint16_t viewport_width, std::uint16_t viewport_height) : _viewport_width(viewport_width), _viewport_height(viewport_height)
	{ }

	virtual glm::mat4 get_view() = 0;

	virtual glm::mat4 get_projection() = 0;

	void set_viewport_width(std::uint16_t viewport_width) { _viewport_width = viewport_width; };
	void set_viewport_height(std::uint16_t viewport_height) { _viewport_height = viewport_height; };

protected:

	std::uint16_t _viewport_width{};
	std::uint16_t _viewport_height{};
};
