#pragma once

#include "states/BaseState.hpp"
#include "blue/Renderer.h"
#include "jobs/MapIntersectionJob.hpp"
#include "terrain/Tile.hpp"

#include <atomic>
#include <Resources.hpp>

class ModelingTerrain : public BaseState
{
public:

	enum class Mode : int
	{
		ELEVATION = 0,
		ADDING_MODELS = 1,
		WATER = 2,
	};

	explicit ModelingTerrain(const bool map_imported);
	~ModelingTerrain();
	std::shared_ptr<BaseState> update() override;
	void on_entry() override;

private:

	struct ModelEntry
	{
		RenderEntityId id;
		Resources::Model model;
		glm::vec3 position;
		glm::vec3 euler;
	};
	std::vector<ModelEntry> entries;


	struct
	{
		RenderEntity entity;
		glm::vec3 euler = {0, 0, 0};
		glm::vec3 position = {0, 0, 0};
	} last_entity;

	std::atomic_bool updated_model{false};
	std::atomic_bool created_water{ false };

	float _water_level = -1.0f;
	float _radius = 0.2f;
	float _model_scale = 1.0f;
	
	Mode _mode = Mode::ELEVATION;
	Resources::Model _model = Resources::Model::PINE_TREE;
	MapIntersectionJob job;
	ImGuiEntityId _window = 0;

	const bool _map_imported;
};
