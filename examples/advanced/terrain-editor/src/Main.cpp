#include <blue/Context.hpp>
#include <blue/Timestep.hpp>
#include <imgui/imgui.h>

#include <cmath>
#include <atomic>

#include "Application.hpp"
#include "Resources.hpp"

int main(int argc, char* argv[])
{
	blue::Context::init();
	BLUE_ASSERT(blue::Context::window().create(900, 600));
	BLUE_ASSERT(blue::Context::gpu_thread().run());
	blue::Context::window().detach_cursor();

	Application::init();
	Resources::init();

	Resources::instance().load_shaders();
	Resources::instance().load_models();
	Resources::instance().load_textures();

	Timestep timestep(30);

	while (Application::instance().is_running())
	{
		timestep.mark_start();
		Application::instance().handle_input();
		timestep.mark_end();
		timestep.delay();
	}

	Application::dispose();
	Resources::dispose();

	blue::Context::gpu_thread().stop();
	blue::Context::dispose();

	return 0;
}
