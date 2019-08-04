#include <blue/Context.hpp>
#include <blue/Timestep.hpp>
#include <imgui/imgui.h>

#include <cmath>
#include <atomic>

#include "Game.hpp"
#include "Resources.hpp"

int main(int argc, char* argv[])
{
	blue::Context::init();
	BLUE_ASSERT(blue::Context::window().create(900, 600));
	BLUE_ASSERT(blue::Context::gpu_thread().run());
	blue::Context::window().detach_cursor();

	Resources::init();
	Game::init();

	Resources::instance().load_shaders();
	Resources::instance().load_models();
	Resources::instance().load_textures();

	Game::instance().get_map().import_from_file("map.bin");
	Game::instance().get_map().upload_clickable_vertices();
	Game::instance().get_map().upload_decoration();

	// Look at the center of map.

	auto& map_environment = Resources::instance().map_environment;

	map_environment.camera.set_pos({ 7.78816, 28.7423, 22.3805 });
	map_environment.camera.set_rotation({ 0, - 68.5f, - 44.25f });

	blue::Context::gpu_system().submit(UpdateEnvironmentEntity_View{ map_environment.environment, map_environment.camera.get_view() });
	blue::Context::gpu_system().submit(UpdateEnvironmentEntity_CameraPos{ map_environment.environment, map_environment.camera.get_position() });

	Timestep timestep(30);

	while (Game::instance().is_running())
	{
		timestep.mark_start();
		Game::instance().handle_input();
		timestep.mark_end();
		timestep.delay();
	}

	Game::dispose();
	Resources::dispose();

	blue::Context::gpu_thread().stop();
	blue::Context::dispose();

	return 0;
}
