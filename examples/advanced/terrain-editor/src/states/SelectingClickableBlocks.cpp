#include "states/SelectingClickableBlocks.hpp"
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

		ImGui::End();
		});
}

SelectingClickableBlocks::~SelectingClickableBlocks()
{
	blue::Context::renderer().remove_imgui_entity({ _blocks_window });
	job.shutdown();
}

std::shared_ptr<BaseState> SelectingClickableBlocks::update()
{
	return nullptr;
}

void SelectingClickableBlocks::on_entry()
{
	// Create map
	Application::instance().get_map().upload_clickable_vertices();
	// Run intersection test job
	job.start();
}
