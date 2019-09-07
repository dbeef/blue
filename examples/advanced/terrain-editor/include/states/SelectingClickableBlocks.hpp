#pragma once

#include "states/BaseState.hpp"
#include "blue/Renderer.h"
#include "jobs/MapIntersectionJob.hpp"
#include "terrain/Tile.hpp"

#include <atomic>

class SelectingClickableBlocks : public BaseState
{
	enum class Mode
	{
		COLOURING,
		CLICKABLE_BLOCKS,
	};

public:

	SelectingClickableBlocks();
	~SelectingClickableBlocks();
	std::shared_ptr<BaseState> update() override;
	void on_entry() override;

private:

	std::atomic_bool _new_level{ false };
	MapIntersectionJob job;
	ImGuiEntityId _blocks_window = 0;
	InterpolationDirection _direction = InterpolationDirection::FULL;
	Mode _mode = Mode::COLOURING;
	float _paint_1[3];
	float _paint_2[3];
};
