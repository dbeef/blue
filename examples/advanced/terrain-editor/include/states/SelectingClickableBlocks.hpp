#pragma once

#include "states/BaseState.hpp"
#include "blue/Renderer.h"
#include "jobs/MapIntersectionJob.hpp"

#include <atomic>

class SelectingClickableBlocks : public BaseState
{
public:

	SelectingClickableBlocks();
	~SelectingClickableBlocks();
	std::shared_ptr<BaseState> update() override;
	void on_entry() override;

private:

	MapIntersectionJob job;
	ImGuiEntityId _blocks_window = 0;
};
