#pragma once

#include "terrain/Map.hpp"
#include "terrain/Flora.hpp"
#include "terrain/Water.hpp"

#include <atomic>
#include <memory>

class Game
{
public:

	static Game& instance();

	static void init();

	static void dispose();

	void shutdown();

	void handle_input();

	bool is_running();

	Map& get_map();

	Flora& get_flora();

	Water& get_water();

	struct {
		std::atomic_bool gesture{ false };
		std::atomic<std::uint32_t> press_x{ 0 };
		std::atomic<std::uint32_t> press_y{ 0 };
		std::atomic<std::uint32_t> release_x{ 0 };
		std::atomic<std::uint32_t> release_y{ 0 };
		std::atomic<std::uint32_t> last_x{ 0 };
		std::atomic<std::uint32_t> last_y{ 0 };
	} input;

	struct {
		std::atomic_bool requested{ false };
		std::atomic_bool found{ false };
		std::atomic<std::uint32_t> x_tiles{ 0 };
		std::atomic<std::uint32_t> y_tiles{ 0 };
		std::atomic<std::uint32_t> x{ 0 };
		std::atomic<std::uint32_t> y{ 0 };
	} intersection;

private:

	std::shared_ptr<Map> _map = std::make_shared<Map>();
	std::shared_ptr<Water> _water = std::make_shared<Water>();
	std::shared_ptr<Flora> _flora = std::make_shared<Flora>();
	std::atomic_bool _running{ true };
	static Game* _instance;
};
