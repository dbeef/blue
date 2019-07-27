#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "glad/glad.h"

using CameraId = std::uint32_t;

class Camera
{
public:

	virtual glm::mat4 get_view() = 0;

	virtual glm::mat4 get_projection() = 0;
};
