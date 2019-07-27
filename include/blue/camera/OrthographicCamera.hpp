#pragma once

#include "blue/camera/Camera.hpp"

class OrthographicCamera : public Camera {

public:

	glm::mat4 get_view() override;

	glm::mat4 get_projection() override;
};
