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
		ADDING_MODELS = 2,
		VERTEX_PAINT_SHUFFLE = 3,
	};

	enum class Model : int
	{
		PINE_TREE = 0,
		TANK = 1,
		HURDLE = 2,
		WHEAT = 3,
		BOULDER = 4,
		SMALL_BOULDER = 5,
		GRASS = 6,
		PYLON = 7,
		BUSH = 8,
		CUT_TREE = 9,
	};

	ModelingTerrain();
	~ModelingTerrain();
	std::shared_ptr<BaseState> update() override;
	void on_entry() override;

private:

	float _radius = 0.2f;
	float _model_scale = 1.0f;
	float _paint[3];
	Mode _mode = Mode::ELEVATION;
	Model _model = Model::PINE_TREE;
	MapIntersectionJob job;
	ImGuiEntityId _window = 0;
};
