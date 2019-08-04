#include "states/SelectingClickableBlocks.hpp"
#include "states/ModelingTerrain.hpp"
#include "Application.hpp"
#include "blue/Context.hpp"
#include "imgui/imgui.h"

#include "terrain/Map.hpp"

SelectingClickableBlocks::SelectingClickableBlocks()
{
	_blocks_window = blue::Context::renderer().add([this]() {
		ImGui::Begin("Blue: Terrain editor [Selecting clickable blocks]");
		
		if (blue::Context::window().is_cursor_attached())
			ImGui::TextColored(ImVec4(1, 0, 0, 1), "Cursor attached, press middle mouse button to detach.");
		else
			ImGui::TextColored(ImVec4(0, 1, 1, 1), "Cursor detached, press middle mouse button to attach.");

		bool new_level = ImGui::Button("I'm done, take me to the modeling step");
		if (new_level)
		{
			_new_level.store(true);
		}

		ImGui::End();
		});
}

SelectingClickableBlocks::~SelectingClickableBlocks()
{
	blue::Context::renderer().remove_imgui_entity({ _blocks_window });
	job.shutdown();
	Application::instance().get_map().dispose_current_map_on_gpu(); // TODO: Also remove vertex arrays
}

std::shared_ptr<BaseState> SelectingClickableBlocks::update()
{
	if (Application::instance().input.intersection.load())
	{
		const auto& x = Application::instance().input.intersection_tile_x;
		const auto& y = Application::instance().input.intersection_tile_y;

		Application::instance().get_map().toggle_tile(x, y);
		Application::instance().get_map().dispose_current_map_on_gpu(); // TODO: Also remove vertex arrays
		Application::instance().get_map().upload_clickable_vertices();
		Application::instance().get_map().upload_decoration_vertices();
		 
		Application::instance().input.intersection.store(false);
	}
	
	if (_new_level.load())
	{
		return std::make_shared<ModelingTerrain>(false);
	}

	return nullptr; 
}

void SelectingClickableBlocks::on_entry()
{
	// Create map
	Application::instance().get_map().upload_clickable_vertices();
	Application::instance().get_map().upload_decoration_vertices();
	// Run intersection test job
	job.start();
}
