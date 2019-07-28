#pragma once

#include "states/BaseState.hpp"
#include "blue/Renderer.h"
#include "jobs/MapIntersectionJob.hpp"

#include <atomic>

class ModelingTerrain : public BaseState
{
public:

	enum class Mode : int
	{
		ELEVATION = 0,
		VERTEX_PAINT = 1,
	};

	ModelingTerrain();
	~ModelingTerrain();
	std::shared_ptr<BaseState> update() override;
	void on_entry() override;

private:

	float _radius = 0;
	float _paint[3];
	Mode _mode = Mode::ELEVATION;
	MapIntersectionJob job;
	ImGuiEntityId _window = 0;
};
