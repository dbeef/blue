#include <blue/Context.hpp>
#include <blue/Timestep.hpp>
#include <imgui/imgui.h>

#include "Callbacks.hpp"
#include "Game.hpp"
#include "Resources.hpp"

#include <cmath>
#include <jobs/MapIntersectionJob.h>

int main(int argc, char* argv[])
{
	blue::Context::init();
	BLUE_ASSERT(blue::Context::window().create(900, 600));
	BLUE_ASSERT(blue::Context::gpu_thread().run());
	blue::Context::window().detach_cursor();

	Resources::init();
	Game::init();
	Callbacks::init();
	Callbacks::instance().register_callbacks();

	Resources::instance().load_environment();
	Resources::instance().load_shaders();
	Resources::instance().load_models();
	Resources::instance().load_textures();
    Resources::instance().load_render_entities();

	Game::instance().get_map().import_from_file("resources/map.bin");
	Game::instance().get_map().upload_decoration();
	Game::instance().get_map().upload_clickable();
	Game::instance().get_flora().import_from_file("resources/flora.bin");
    Game::instance().get_water().import_from_file("resources/water.bin");
	Game::instance().get_water().create_water(Game::instance().get_map());

	blue::Context::gpu_system().submit(SetClearColorEntity{ {1.0f, 1.0f, 1.0f} });

    MapIntersectionJob job;
    job.start();

    Timestep timestep(30);

	while (Game::instance().is_running())
	{
		timestep.mark_start();
		Game::instance().handle_input();

		static float x = 0.0f;
		x += 0.05f;
		float sin = std::sin(x) * 0.2f;
		auto swing_update = UpdateUniformVariableEntity{ ShaderAttribute::Type::FLOAT, &sin, Resources::instance().shaders.swinging, 1, "" };
		blue::Context::gpu_system().submit(swing_update);

		timestep.mark_end();
		timestep.delay();
	}

    job.shutdown();

	Callbacks::dispose();
	Game::dispose();
	Resources::dispose();

	blue::Context::gpu_thread().stop();
	blue::Context::dispose();

	return 0;
}
