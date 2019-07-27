//
// Created by dbeef on 3/25/19.
//

#include "blue/camera/PerspectiveCamera.hpp"
#include "blue/Context.hpp"

void PerspectiveCamera::set_pos(const glm::vec3& pos)
{
	_position = pos;
}

glm::mat4 PerspectiveCamera::get_view()
{
	return glm::lookAt(_position, _position + _front, _CAMERA_UP);
}

glm::mat4 PerspectiveCamera::get_projection()
{
	return glm::perspective(glm::radians(_fov), _aspect, _near, _far);
}

void PerspectiveCamera::reset()
{
	// Yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a
	// direction vector pointing to the right (due to how Eular angles work)
	// so we initially rotate a bit to the left.
	_yaw = -90.0f;
	_pitch = 0;
	_lastX = static_cast<GLfloat>(blue::Context::window().get_width() / 2.0);
	_lastY = static_cast<GLfloat>(blue::Context::window().get_height() / 2.0);
	_fov = 45.0f;

	// For calculating right-vector.
	_aspect = static_cast<float>(blue::Context::window().get_width()) / static_cast<float>(blue::Context::window().get_height());
}

void PerspectiveCamera::go_forward(float distance)
{
	_position += _front * distance;
}

void PerspectiveCamera::go_backward(float distance)
{
	_position -= _front * distance;
}

void PerspectiveCamera::go_left(float distance)
{
	_position -= glm::normalize(glm::cross(_front, _CAMERA_UP)) * distance;
}

void PerspectiveCamera::go_right(float distance)
{
	_position += glm::normalize(glm::cross(_front, _CAMERA_UP)) * distance;
}

void PerspectiveCamera::mouse_rotation(double xpos, double ypos)
{
	GLfloat xoffset = xpos - _lastX;
	GLfloat yoffset = _lastY - ypos; // Reversed since y-coordinates go from bottom to left

	GLfloat sensitivity = 0.25;    // Change this value to your liking
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	_yaw += xoffset;
	_pitch += yoffset;

	// Make sure that when pitch is out of bounds, screen doesn't get flipped
	if (_pitch > 89.0f)
		_pitch = 89.0f;
	if (_pitch < -89.0f)
		_pitch = -89.0f;

	_front.x = cos(glm::radians(_yaw)) * cos(glm::radians(_pitch));
	_front.y = sin(glm::radians(_pitch));
	_front.z = sin(glm::radians(_yaw)) * cos(glm::radians(_pitch));
	_front = glm::normalize(_front);
}