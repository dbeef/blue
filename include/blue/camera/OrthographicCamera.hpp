#pragma once

#include "blue/camera/Camera.hpp"

class OrthographicCamera : public Camera {

public:

    enum class Mode
    {
        CLIP_SPACE,  // for 3d rendering
        SCREEN_SPACE // for 2d rendering
    };

	explicit OrthographicCamera(Mode mode, std::uint16_t viewport_width, std::uint16_t viewport_height) : 
		_mode(mode), Camera(viewport_width, viewport_height)
	{
		reset();
	}

	void reset();

	glm::mat4 get_view() override;

	glm::mat4 get_projection() override;

	void set_pos(const glm::vec3&);

	void set_rotation(const glm::vec3& euler);

	// moving

	void go_forward(float distance);

	void go_backward(float distance);

	void go_left(float distance);

	void go_right(float distance);

	void mouse_rotation(double xpos, double ypos);

	// getters

	float get_roll() const;

	float get_pitch() const;

	float get_yaw() const;

	float get_fov() const;

	glm::vec3 get_position() const;

	glm::vec3 get_front() const;

	glm::vec3 get_up() const;

	GLfloat get_last_x() const;

	GLfloat get_last_y() const;

	void set_near(float);

	void set_far(float);

private:

    Mode _mode;

	// roll is always 0
	GLfloat _yaw = 0;
	GLfloat _pitch = 0;
	GLfloat _lastX = 0;
	GLfloat _lastY = 0;
	GLfloat _fov = 0;
	float _aspect{}; // (view ratio)

	glm::vec3 _position = glm::vec3(0.0f, 0.0f, 10.0f);
	glm::vec3 _front = glm::vec3(0.0f, 0.0f, -1.0f);

	glm::vec3 _HELPER_CAMERA_TARGET = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 _HELPER_CAMERA_DIRECTION = glm::normalize(_position - _HELPER_CAMERA_TARGET);
	glm::vec3 _HELPER_UP = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 _CAMERA_RIGHT = glm::normalize(glm::cross(_HELPER_UP, _HELPER_CAMERA_DIRECTION));
	glm::vec3 _CAMERA_UP = glm::cross(_HELPER_CAMERA_DIRECTION, _CAMERA_RIGHT);

	float _near = -1;
	float _far = 1;
};
