//
// Created by dbeef on 3/25/19.
//

#include "blue/PerspectiveCamera.hpp"
#include "blue/Context.hpp"

glm::mat4 PerspectiveCamera::get_view()
{
	return glm::lookAt(cameraPos, cameraPos + cameraFront, CAMERA_UP);
}

glm::mat4 PerspectiveCamera::get_projection()
{
	return glm::perspective(glm::radians(fov), aspect, near, far);
}

void PerspectiveCamera::reset()
{
	// Yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a
	// direction vector pointing to the right (due to how Eular angles work)
	// so we initially rotate a bit to the left.
	yaw = -90.0f;
	pitch = 0;
	lastX = static_cast<GLfloat>(blue::Context::window().get_width() / 2.0);
	lastY = static_cast<GLfloat>(blue::Context::window().get_height() / 2.0);
	fov = 45.0f;

	// For calculating right-vector.
	aspect = static_cast<float>(blue::Context::window().get_width()) / static_cast<float>(blue::Context::window().get_height());
}
