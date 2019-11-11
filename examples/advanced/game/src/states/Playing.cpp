#include <states/Playing.hpp>
#include <Game.hpp>

namespace
{
	static const float CAMERA_SPEED = 0.25f;

	void limit_cam_boundaries(glm::vec3& pos)
	{
		float radius = 20.0f;
		glm::vec3 center = { 32.0f, pos.y, 32.0f };

		const auto distance = glm::distance(pos, center);
		if (distance <= radius)
		{
			// OK, still in boundaries
		}
		else
		{
			// Calculate direction, set position to be direction times radius.
			const auto direction = glm::normalize(pos - center);
			pos = center + (direction * radius);
			//pos = direction * radius;
			//pos.y = center.y;
		}
	}

	void register_callbacks()
	{
		blue::Context::input().registerKeyCallback({
														   []() { Game::instance().shutdown(); },
														   SDLK_ESCAPE,
														   SDL_KEYDOWN
			}
		);

		auto w_callback = []()
		{
			auto& map_environment = Resources::instance().map_environment;
			map_environment.camera.go_forward(CAMERA_SPEED);
			blue::Context::gpu_system().submit(UpdateEnvironmentEntity_View{ map_environment.environment, map_environment.camera.get_view() });
			blue::Context::gpu_system().submit(UpdateEnvironmentEntity_CameraPos{ map_environment.environment, map_environment.camera.get_position() });
		};
		blue::Context::input().registerKeyCallback({ w_callback, SDLK_w, SDL_KEYDOWN });

		auto mouse_middle_callback = [](double x, double y)
		{
			if (blue::Context::window().is_cursor_attached())
			{
				blue::Context::window().detach_cursor();
			}
			else
			{
				blue::Context::window().attach_cursor();
			}
		};
		blue::Context::input().registerMouseKeyCallback({ mouse_middle_callback, SDL_BUTTON_MIDDLE, SDL_MOUSEBUTTONDOWN });

		auto mouse_left_press_callback = [](double xpos, double ypos)
		{
			blue::Context::logger().info("Press on: {} {}", xpos, ypos);
			Game::instance().input.last_x = xpos;
			Game::instance().input.last_y = ypos;
			Game::instance().input.gesture.store(true);
			Game::instance().intersection.requested.store(true);
		};
		blue::Context::input().registerMouseKeyCallback({ mouse_left_press_callback, SDL_BUTTON_LEFT, SDL_MOUSEBUTTONDOWN });

		auto mouse_left_release_callback = [](double xpos, double ypos)
		{
			blue::Context::logger().info("Release on: {} {}", xpos, ypos);
			Game::instance().input.last_x = xpos;
			Game::instance().input.last_y = ypos;
			Game::instance().input.gesture.store(false);
		};
		blue::Context::input().registerMouseKeyCallback({ mouse_left_release_callback, SDL_BUTTON_LEFT, SDL_MOUSEBUTTONUP });

		auto s_callback = []()
		{
			auto& map_environment = Resources::instance().map_environment;
			map_environment.camera.go_backward(CAMERA_SPEED);
			blue::Context::gpu_system().submit(UpdateEnvironmentEntity_View{ map_environment.environment, map_environment.camera.get_view() });
			blue::Context::gpu_system().submit(UpdateEnvironmentEntity_CameraPos{ map_environment.environment, map_environment.camera.get_position() });
		};
		blue::Context::input().registerKeyCallback({ s_callback, SDLK_s, SDL_KEYDOWN });

		auto a_callback = []()
		{
			auto& map_environment = Resources::instance().map_environment;
			map_environment.camera.go_left(CAMERA_SPEED);
			blue::Context::gpu_system().submit(UpdateEnvironmentEntity_View{ map_environment.environment, map_environment.camera.get_view() });
			blue::Context::gpu_system().submit(UpdateEnvironmentEntity_CameraPos{ map_environment.environment, map_environment.camera.get_position() });
		};
		blue::Context::input().registerKeyCallback({ a_callback, SDLK_a, SDL_KEYDOWN });

		auto d_callback = []()
		{
			auto& map_environment = Resources::instance().map_environment;
			map_environment.camera.go_right(CAMERA_SPEED);
			blue::Context::gpu_system().submit(UpdateEnvironmentEntity_View{ map_environment.environment, map_environment.camera.get_view() });
			blue::Context::gpu_system().submit(UpdateEnvironmentEntity_CameraPos{ map_environment.environment, map_environment.camera.get_position() });
		};
		blue::Context::input().registerKeyCallback({ d_callback, SDLK_d, SDL_KEYDOWN });

		auto left_arrow_callback = []()
		{
			const float& delta_rotation_rad = 2.5f;
			auto& map_environment = Resources::instance().map_environment;
			map_environment.camera.add_rotation(delta_rotation_rad, 0);
			blue::Context::gpu_system().submit(UpdateEnvironmentEntity_View{ map_environment.environment, map_environment.camera.get_view() });
		};
		blue::Context::input().registerKeyCallback({ left_arrow_callback, SDLK_LEFT, SDL_KEYDOWN });

		auto right_arrow_callback = []()
		{
			const float& delta_rotation_rad = -2.5f;
			auto& map_environment = Resources::instance().map_environment;
			map_environment.camera.add_rotation(delta_rotation_rad, 0);
			blue::Context::gpu_system().submit(UpdateEnvironmentEntity_View{ map_environment.environment, map_environment.camera.get_view() });
		};
		blue::Context::input().registerKeyCallback({ right_arrow_callback, SDLK_RIGHT, SDL_KEYDOWN });

		auto mouse_move_callback = [](double xpos, double ypos)
		{
			auto& map_environment = Resources::instance().map_environment;

			if (blue::Context::window().is_cursor_attached())
			{
				map_environment.camera.mouse_rotation(xpos, ypos);
			}

			blue::Context::gpu_system().submit(UpdateEnvironmentEntity_View{ map_environment.environment, map_environment.camera.get_view() });
			blue::Context::gpu_system().submit(UpdateEnvironmentEntity_CameraPos{ map_environment.environment, map_environment.camera.get_position() });

			if (Game::instance().input.gesture.load())
			{
				glm::vec2 origin = { Game::instance().input.last_x.load() , Game::instance().input.last_y.load() };
				glm::vec2 target = { xpos, ypos };
				glm::vec2 delta = glm::normalize(origin - target);

				const auto front = glm::normalize(map_environment.camera.get_front());
				const auto up = glm::normalize(glm::cross(front, map_environment.camera.get_up()));
				auto left = glm::normalize(glm::cross(front, up));

				auto pos = map_environment.camera.get_position();

				glm::vec3 cam_delta = pos;
				cam_delta += 0.25f * delta.x * up;
				cam_delta += 0.25f * delta.y * left;
				cam_delta.y = pos.y;

				limit_cam_boundaries(cam_delta);

				map_environment.camera.set_pos(cam_delta);

				blue::Context::gpu_system().submit(UpdateEnvironmentEntity_View{ map_environment.environment, map_environment.camera.get_view() });
				blue::Context::gpu_system().submit(UpdateEnvironmentEntity_CameraPos{ map_environment.environment, map_environment.camera.get_position() });

				Game::instance().input.last_x = xpos;
				Game::instance().input.last_y = ypos;
			}

			const auto& camera = Resources::instance().map_environment.camera;
		};

		blue::Context::input().registerMouseMoveCallback(mouse_move_callback);

		auto gesture_callback = [](SDL_MultiGestureEvent event)
		{
			const float& delta_rotation_rad = event.dTheta;
			auto& map_environment = Resources::instance().map_environment;
			map_environment.camera.add_rotation(delta_rotation_rad * 10, 0);
			blue::Context::gpu_system().submit(UpdateEnvironmentEntity_View{ map_environment.environment, map_environment.camera.get_view() });
		};

		blue::Context::input().registerGestureCallback(gesture_callback);
	}
}

std::shared_ptr <BaseState> Playing::update()
{
	return nullptr;
}

void Playing::on_entry()
{
	Game::instance().get_map().import_from_file("resources/map.bin");
	Game::instance().get_map().upload_decoration();
	Game::instance().get_map().upload_clickable();
	Game::instance().get_flora().import_from_file("resources/flora.bin");
	Game::instance().get_water().import_from_file("resources/water.bin");
	Game::instance().get_water().create_water(Game::instance().get_map());

	register_callbacks();

	_intersection_job.start();
	_framerate_rendering_job.start();
}

Playing::~Playing()
{
	_intersection_job.shutdown();
	_framerate_rendering_job.shutdown();
}
