#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "glad/glad.h"

using CameraId = std::uint32_t;

//A normalized vector is one which is often used just to denote pure directions
// without bothering about the magnitude (set to 1; hence their other, more common name unit vector)
// i.e. how far the vector pushes doesn't matter but in what direction does it point/push matters.

// FIXME: Quaternions may be faster than rotation matrices
class PerspectiveCamera {

public:

	PerspectiveCamera()
	{
		reset();
	}

	void reset();

	glm::mat4 get_view();

	glm::mat4 get_projection();

	// roll is always 0
	GLfloat yaw = 0;
	GLfloat pitch = 0;
	GLfloat lastX = 0;
	GLfloat lastY = 0;
	GLfloat fov = 0;

	glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 10.0f);
	glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);

	const glm::vec3 HELPER_CAMERA_TARGET = glm::vec3(0.0f, 0.0f, 0.0f);
	const glm::vec3 HELPER_CAMERA_DIRECTION = glm::normalize(cameraPos - HELPER_CAMERA_TARGET);
	const glm::vec3 HELPER_UP = glm::vec3(0.0f, 1.0f, 0.0f);
	const glm::vec3 CAMERA_RIGHT = glm::normalize(glm::cross(HELPER_UP, HELPER_CAMERA_DIRECTION));
	const glm::vec3 CAMERA_UP = glm::cross(HELPER_CAMERA_DIRECTION, CAMERA_RIGHT);

	const float near = 0.1f;
	const float far = 500.0f;

	float aspect{}; // (view ratio)
};