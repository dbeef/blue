#pragma once

#include "states/BaseState.hpp"

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

private:

	std::shared_ptr<BaseState> _current_state = nullptr;
	std::atomic_bool _running{ true };
	static Application* _instance;
};
