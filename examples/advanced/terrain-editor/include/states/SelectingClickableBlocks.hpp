#pragma once

#include "states/BaseState.hpp"
#include "blue/Renderer.h"

#include <atomic>

class SelectingClickableBlocks : public BaseState
{
public:

	SelectingClickableBlocks();
	~SelectingClickableBlocks();
	std::shared_ptr<BaseState> update() override;

private:

	ImGuiEntityId _blocks_window = 0;
};
