#pragma once

#include "states/BaseState.hpp"
#include "terrain/Map.hpp"
#include "terrain/Water.hpp"
#include "terrain/Flora.hpp"

#include <atomic>
#include <memory>

class Application
{
public:

	Application();

	static Application& instance();

	static void init();
	
	static void dispose();

	void shutdown();

	void handle_input();

	bool is_running();

	Map& get_map();
    Flora& get_flora();
    Water& get_water();

	struct {
		std::atomic_bool clicked{ false };
		std::atomic_int clicked_button { 0 };
		std::atomic_bool intersection {false};
		std::atomic<uint16_t> intersection_tile_x {0};
		std::atomic<uint16_t> intersection_tile_y {0};
		// It's assumed that y = 0 on intersection point.
		std::atomic<double> intersection_point_x {0};
		std::atomic<double> intersection_point_y {0};
	} input;

private:

	void register_callbacks();

    std::shared_ptr<Flora> _flora = std::make_shared<Flora>();
    std::shared_ptr<Water> _water = std::make_shared<Water>();
    std::shared_ptr<Map> _map = std::make_shared<Map>();
	std::shared_ptr<BaseState> _current_state = nullptr;
	std::atomic_bool _running{ true };
	static Application* _instance;
};
