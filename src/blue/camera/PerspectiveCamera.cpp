//
// Created by dbeef on 3/25/19.
//

#include "blue/camera/PerspectiveCamera.hpp"
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

void PerspectiveCamera::go_forward(float distance)
{
	cameraPos += cameraFront * distance;
}

void PerspectiveCamera::go_backward(float distance)
{
	cameraPos -= cameraFront * distance;
}

void PerspectiveCamera::go_left(float distance)
{
	cameraPos -= glm::normalize(glm::cross(cameraFront, CAMERA_UP)) * distance;
}

void PerspectiveCamera::go_right(float distance)
{
	cameraPos += glm::normalize(glm::cross(cameraFront, CAMERA_UP)) * distance;
}

void PerspectiveCamera::mouse_rotation(double xpos, double ypos)
{
	GLfloat xoffset = xpos - lastX;
	GLfloat yoffset = lastY - ypos; // Reversed since y-coordinates go from bottom to left

	GLfloat sensitivity = 0.25;    // Change this value to your liking
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	// Make sure that when pitch is out of bounds, screen doesn't get flipped
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);
}