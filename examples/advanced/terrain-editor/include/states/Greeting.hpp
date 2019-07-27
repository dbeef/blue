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

private:

	std::atomic_bool _new_level{ false };
	ImGuiEntityId _greeting_window = 0;
};
