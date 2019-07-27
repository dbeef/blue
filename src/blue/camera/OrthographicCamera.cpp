#include "blue/camera/OrthographicCamera.hpp"
#include "blue/Context.hpp"

void OrthographicCamera::reset()
{
}

glm::mat4 OrthographicCamera::get_view()
{
	return glm::identity<glm::mat4>();
}

glm::mat4 OrthographicCamera::get_projection()
{
	return glm::ortho(
		0.0f,
		static_cast<float>(blue::Context::window().get_width()),
		static_cast<float>(blue::Context::window().get_height()),
		0.0f,
		-1.0f,
		1.0f);
}

