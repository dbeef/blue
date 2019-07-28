#pragma once

#include "states/BaseState.hpp"
#include "terrain/Map.hpp"

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

	struct {
		UniformBufferId environment = 0;
		PerspectiveCamera camera;
	} map_environment;

	struct {
		std::atomic_bool clicked{ false };
		std::atomic_bool intersection {false};
		std::atomic_uint16_t intersection_tile_x {0};
		std::atomic_uint16_t intersection_tile_y {0};
		// It's assumed that y = 0 on intersection point.
		std::atomic_uint16_t intersection_point_x {0};
		std::atomic_uint16_t intersection_point_y {0};
	} input;

private:

	void register_callbacks();

	std::shared_ptr<Map> _map = std::make_shared<Map>();
	std::shared_ptr<BaseState> _current_state = nullptr;
	std::atomic_bool _running{ true };
	static Application* _instance;
};
