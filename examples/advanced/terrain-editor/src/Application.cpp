#include "Application.hpp"
#include "states/Greeting.hpp"

#include "blue/Assertions.h"
#include "blue/Context.hpp"

#include "imgui/imgui.h"

const float CAMERA_SPEED = 0.25f;

Application* Application::_instance = nullptr;

Application::Application()
{
	map_environment.camera.set_pos({ 10, 10, 10 });

	// Create map_environment

	auto environment_future = blue::Context::gpu_system().submit(CreateEnvironmentEntity{});
	environment_future.wait();
	map_environment.environment = environment_future.get();

	// Upload map_environment.camera's matrices

	blue::Context::gpu_system().submit(UpdateEnvironmentEntity_Projection{ map_environment.environment, map_environment.camera.get_projection() });
	blue::Context::gpu_system().submit(UpdateEnvironmentEntity_View{ map_environment.environment, map_environment.camera.get_view() });

	register_callbacks();

	blue::Context::gpu_system().submit(SetClearColorEntity{ {0.25f, 0.45f, 0.8f} });
	_current_state = std::make_shared<Greeting>();
}

Application& Application::instance()
{
	BLUE_ASSERT(_instance);
	return *_instance;
}

void Application::init()
{
	BLUE_ASSERT(!_instance);
	_instance = new Application();
}

void Application::dispose()
{
	BLUE_ASSERT(_instance);
	delete _instance;
	_instance = nullptr;
}

void Application::shutdown()
{
	_running.store(false);
}

void Application::handle_input()
{
	blue::Context::input().poll();

	auto next_state = _current_state->update();
	if (next_state)
	{
		_current_state = next_state;
		_current_state->on_entry();
	}
}

bool Application::is_running()
{
	return _running.load();
}

Map& Application::get_map()
{
	return *_map.get();
}

void Application::register_callbacks()
{
	blue::Context::input().registerKeyCallback({
			[]() { Application::instance().shutdown(); },
			SDLK_ESCAPE,
			SDL_KEYDOWN
		}
	);

	auto w_callback = [this]()
	{
		map_environment.camera.go_forward(CAMERA_SPEED);
		blue::Context::gpu_system().submit(UpdateEnvironmentEntity_View{ map_environment.environment, map_environment.camera.get_view() });
	};
	blue::Context::input().registerKeyCallback({ w_callback, SDLK_w, SDL_KEYDOWN });
	
	auto mouse_middle_callback = [this]()
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

	auto mouse_left_callback = [this]()
	{
		if (!blue::Context::window().is_cursor_attached())
		{
			Application::instance().input.clicked.store(true);
			Application::instance().input.clicked_button.store(SDL_BUTTON_LEFT);
		}
	};
	blue::Context::input().registerMouseKeyCallback({ mouse_left_callback, SDL_BUTTON_LEFT, SDL_MOUSEBUTTONDOWN });
	
	auto mouse_right_callback = [this]()
	{
		if (!blue::Context::window().is_cursor_attached())
		{
			Application::instance().input.clicked.store(true);
			Application::instance().input.clicked_button.store(SDL_BUTTON_RIGHT);
		}
	};
	blue::Context::input().registerMouseKeyCallback({ mouse_right_callback, SDL_BUTTON_RIGHT, SDL_MOUSEBUTTONDOWN });

	auto s_callback = [this]()
	{
		map_environment.camera.go_backward(CAMERA_SPEED);
		blue::Context::gpu_system().submit(UpdateEnvironmentEntity_View{ map_environment.environment, map_environment.camera.get_view() });
	};
	blue::Context::input().registerKeyCallback({ s_callback, SDLK_s, SDL_KEYDOWN });

	auto a_callback = [this]()
	{
		map_environment.camera.go_left(CAMERA_SPEED);
		blue::Context::gpu_system().submit(UpdateEnvironmentEntity_View{ map_environment.environment, map_environment.camera.get_view() });
	};
	blue::Context::input().registerKeyCallback({ a_callback, SDLK_a, SDL_KEYDOWN });

	auto d_callback = [this]()
	{
		map_environment.camera.go_right(CAMERA_SPEED);
		blue::Context::gpu_system().submit(UpdateEnvironmentEntity_View{ map_environment.environment, map_environment.camera.get_view() });
	};
	blue::Context::input().registerKeyCallback({ d_callback, SDLK_d, SDL_KEYDOWN });

	auto mouse_move_callback = [this](double xpos, double ypos)
	{
		if (blue::Context::window().is_cursor_attached())
		{ 
			map_environment.camera.mouse_rotation(xpos, ypos);
		}

		blue::Context::gpu_system().submit(UpdateEnvironmentEntity_View{ map_environment.environment, map_environment.camera.get_view() });
	};
	blue::Context::input().registerMouseMoveCallback(mouse_move_callback);
}
