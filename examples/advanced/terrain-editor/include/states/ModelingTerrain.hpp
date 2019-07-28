#pragma once

#include "states/BaseState.hpp"
#include "blue/Renderer.h"
#include "jobs/MapIntersectionJob.hpp"

#include <atomic>

class ModelingTerrain : public BaseState
{
public:

	ModelingTerrain();
	~ModelingTerrain();
	std::shared_ptr<BaseState> update() override;
	void on_entry() override;

private:

	//std::atomic_bool _new_level{ false };
	//MapIntersectionJob job;
	ImGuiEntityId _window = 0;
};
