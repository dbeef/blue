#include <blue/Context.hpp>
#include <blue/Timestep.hpp>
#include <imgui/imgui.h>

#include <cmath>
#include <atomic>
#include <Resources.hpp>

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

    auto& map_environment = Resources::instance().map_environment;
    auto& light_environment = Resources::instance().light_environment;

    map_environment.camera.set_pos({ 7.78816, 28.7423, 22.3805 });
    map_environment.camera.set_rotation({ 0, - 68.5f, - 44.25f });

	light_environment.camera.set_far(500.0f);
	light_environment.camera.set_near(-100.0f);
	light_environment.camera.set_pos({ 64.0f, 20.4532f, 64.0f});
//	light_environment.camera.look_at({64.0f, 0.0f, 64.0f});

	blue::Context::gpu_system().submit(UpdateEnvironmentEntity_View{ map_environment.environment, map_environment.camera.get_view() });
    blue::Context::gpu_system().submit(UpdateEnvironmentEntity_CameraPos{ map_environment.environment, map_environment.camera.get_position() });

	// Upload light-space matrices to both environments:
	blue::Context::gpu_system().submit(UpdateEnvironmentEntity_Projection{ light_environment.environment, light_environment.camera.get_projection() });
	blue::Context::gpu_system().submit(UpdateEnvironmentEntity_View{ light_environment.environment, light_environment.camera.get_view() });
	blue::Context::gpu_system().submit(UpdateEnvironmentEntity_LightSpaceMatrix{ map_environment.environment,
																			  light_environment.camera.get_projection() * light_environment.camera.get_view() });

	blue::Context::gpu_system().submit(UpdateEnvironmentEntity_LightPos{ map_environment.environment, light_environment.camera.get_position() });

	Resources::instance().load_shaders();
	Resources::instance().load_models();
	Resources::instance().load_textures();

    Game::instance().get_map().import_from_file("resources/map.bin");
//    Game::instance().get_map().upload_clickable_vertices(); // FIXME?
    Game::instance().get_map().upload_decoration();

    Game::instance().get_flora().import_from_file("resources/flora.bin");

    // Look at the center of map.

	Timestep timestep(30);

	while (Game::instance().is_running())
	{
		timestep.mark_start();
		Game::instance().handle_input();

		static float x = 0.0f;
		x += 0.05f;
		float sin = std::sin(x) * 0.2f;
		blue::Context::gpu_system().submit(UpdateUniformVariableEntity{ ShaderAttribute::Type::FLOAT, &sin, Resources::instance().shaders.swinging_shader, 1, "" });

		timestep.mark_end();
		timestep.delay();
	}

	Game::dispose();
	Resources::dispose();

	blue::Context::gpu_thread().stop();
	blue::Context::dispose();

	return 0;
}
