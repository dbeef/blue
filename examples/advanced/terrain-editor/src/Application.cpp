#include "Application.hpp"
#include "states/Greeting.hpp"

#include "blue/Assertions.h"
#include "blue/Context.hpp"

#include "imgui/imgui.h"

Application* Application::_instance = nullptr;

Application::Application()
{
	blue::Context::input().registerKeyCallback({
			[]() { Application::instance().shutdown(); },
			SDLK_ESCAPE,
			SDL_KEYDOWN
		}
	);

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
