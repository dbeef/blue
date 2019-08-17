#include "Resources.hpp"
#include "Game.hpp"

#include <blue/Assertions.h>
#include <blue/Context.hpp>
#include <imgui/imgui.h>

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
