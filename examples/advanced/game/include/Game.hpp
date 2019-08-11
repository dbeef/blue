#pragma once

#include "terrain/Map.hpp"

#include <atomic>
#include <memory>
#include <terrain/Flora.hpp>

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

	Flora& get_flora();

	struct {
		std::atomic_bool gesture{ false };
		std::atomic<std::uint32_t> press_x{ 0 };
		std::atomic<std::uint32_t> press_y{ 0 };
		std::atomic<std::uint32_t> release_x{ 0 };
		std::atomic<std::uint32_t> release_y{ 0 };
		std::atomic<std::uint32_t> last_x{ 0 };
		std::atomic<std::uint32_t> last_y{ 0 };
	} input;

private:

	void register_callbacks();

	std::shared_ptr<Map> _map = std::make_shared<Map>();
	std::shared_ptr<Flora> _flora = std::make_shared<Flora>();
	std::atomic_bool _running{ true };
	static Game* _instance;
};
