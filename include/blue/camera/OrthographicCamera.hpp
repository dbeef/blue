#pragma once

#include "blue/camera/Camera.hpp"

class OrthographicCamera : public Camera {

public:

	OrthographicCamera()
	{
		reset();
	}

	void reset() override;

	glm::mat4 get_view() override;

	glm::mat4 get_projection() override;
};
