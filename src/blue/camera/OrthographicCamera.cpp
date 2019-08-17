#include "blue/camera/OrthographicCamera.hpp"
#include "blue/Context.hpp"

glm::mat4 OrthographicCamera::get_view()
{
    if(_mode == Mode::SCREEN_SPACE) {
        return glm::identity<glm::mat4>();
    }
    else
    {
        return glm::lookAt(
                _position,
                _front,
                _HELPER_UP);
    }
}

glm::mat4 OrthographicCamera::get_projection()
{
    if(_mode == Mode::SCREEN_SPACE) 
    {
        return glm::ortho(
                0.0f,
                static_cast<float>(blue::Context::window().get_width()),
                static_cast<float>(blue::Context::window().get_height()),
                0.0f,
                _near,
                _far);
    }
    else
    {
        float base = 25.0f;
        return glm::ortho(-1.0f * base, 1.0f * base, -1.0f * base, 1.0f * base, -100.0f, 500.0f);
    }
}

void OrthographicCamera::set_near(float near)
{
	_near = near;
}

void OrthographicCamera::set_far(float far)
{
	_far = far;
}

void OrthographicCamera::set_pos(const glm::vec3& pos)
{
	_position = pos;
	blue::Context::logger().info("Position set to: {} {} {}", pos.x, pos.y, pos.z);
}

void OrthographicCamera::set_rotation(const glm::vec3& euler)
{
	_yaw += euler.x;
	_pitch += euler.y;

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

void OrthographicCamera::reset()
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

void OrthographicCamera::go_forward(float distance)
{
	_position += _front * distance;
}

void OrthographicCamera::go_backward(float distance)
{
	_position -= _front * distance;
}

void OrthographicCamera::go_left(float distance)
{
	_position -= glm::normalize(glm::cross(_front, _CAMERA_UP)) * distance;
}

void OrthographicCamera::go_right(float distance)
{
	_position += glm::normalize(glm::cross(_front, _CAMERA_UP)) * distance;
}

void OrthographicCamera::mouse_rotation(double xpos, double ypos)
{
	GLfloat xoffset = xpos - static_cast<GLfloat>(blue::Context::window().get_width() / 2.0);
	GLfloat yoffset = static_cast<GLfloat>(blue::Context::window().get_height() / 2.0) - ypos; // Reversed since y-coordinates go from bottom to left

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

	_lastX = xpos;
	_lastY = ypos;
}

float OrthographicCamera::get_roll() const
{
	// Always zero.
	return 0;
}

float OrthographicCamera::get_pitch() const
{
	return _pitch;
}

float OrthographicCamera::get_yaw() const
{
	return _yaw;
}

float OrthographicCamera::get_fov() const
{
	return _fov;
}

glm::vec3 OrthographicCamera::get_position() const
{
	return _position;
}

glm::vec3 OrthographicCamera::get_front() const
{
	return _front;
}

glm::vec3 OrthographicCamera::get_up() const
{
	return _HELPER_UP;
}

GLfloat OrthographicCamera::get_last_x() const
{
	return _lastX;
}

GLfloat OrthographicCamera::get_last_y() const
{
	return _lastY;
}
