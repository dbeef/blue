#pragma once

#include "states/BaseState.hpp"
#include "blue/Renderer.h"

#include <atomic>

class Greeting : public BaseState
{
public:

	Greeting();
	~Greeting();
	std::shared_ptr<BaseState> update() override;
	void on_entry() override;

private:

	std::atomic_bool _new_level{ false };
	std::atomic_bool _load_level{ false };
	ImGuiEntityId _greeting_window = 0;
};
