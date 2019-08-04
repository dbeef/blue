#pragma once

#include "terrain/Map.hpp"

#include <atomic>
#include <memory>

class Game
{
public:

	Game();

	static Game& instance();

	static void init();

	static void dispose();

	void shutdown();

	void handle_input();

	bool is_running();

	Map& get_map();

	struct {
		std::atomic_bool gesture{ false };
		std::atomic_uint32_t press_x{ 0 };
		std::atomic_uint32_t press_y{ 0 };
		std::atomic_uint32_t release_x{ 0 };
		std::atomic_uint32_t release_y{ 0 };
		std::atomic_uint32_t last_x{ 0 };
		std::atomic_uint32_t last_y{ 0 };
	} input;

private:

	void register_callbacks();

	std::shared_ptr<Map> _map = std::make_shared<Map>();
	std::atomic_bool _running{ true };
	static Game* _instance;
};
