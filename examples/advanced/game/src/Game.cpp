#include <blue/Assertions.h>
#include <blue/Context.hpp>
#include <imgui/imgui.h>
#include <utility>

#include "Resources.hpp"
#include "Game.hpp"

Game* Game::_instance = nullptr;

Game& Game::instance()
{
	BLUE_ASSERT(_instance);
	return *_instance;
}

void Game::init()
{
	BLUE_ASSERT(!_instance);
	_instance = new Game();
}

void Game::dispose()
{
	BLUE_ASSERT(_instance);
	delete _instance;
	_instance = nullptr;
}

void Game::shutdown()
{
	_running.store(false);
}

void Game::handle_input()
{
	blue::Context::input().poll();

	auto next_state = _current_state->update();
	if (next_state)
	{
		_current_state = next_state;
		_current_state->on_entry();
	}
}

bool Game::is_running()
{
	return _running.load();
}

Map& Game::get_map()
{
	return *_map.get();
}

Flora &Game::get_flora()
{
    return *_flora;
}

Water& Game::get_water()
{
	return *_water;
}

void Game::enter_state(std::shared_ptr<BaseState> state)
{
    _current_state = std::move(state);
    _current_state->on_entry();
}

void Game::reset_state()
{
	_current_state.reset();
}
