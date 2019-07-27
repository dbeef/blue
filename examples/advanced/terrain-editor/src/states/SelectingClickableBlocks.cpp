#include "states/SelectingClickableBlocks.hpp"
#include "blue/Context.hpp"
#include "imgui/imgui.h"

#include "terrain/Map.hpp"

SelectingClickableBlocks::SelectingClickableBlocks()
{
	_blocks_window = blue::Context::renderer().add([this]() {
		ImGui::Begin("Blue: Terrain editor [Selecting clickable blocks]");
		ImGui::End();
		});
}

SelectingClickableBlocks::~SelectingClickableBlocks()
{
	blue::Context::renderer().remove_imgui_entity({ _blocks_window });
}

std::shared_ptr<BaseState> SelectingClickableBlocks::update()
{
	return nullptr;
}

void SelectingClickableBlocks::on_entry()
{
	// Create map
	
	static auto map = std::make_shared<Map>();
	map->upload_clickable_vertices();

	// Run intersection test job

}
