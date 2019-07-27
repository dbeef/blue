#include "states/SelectingClickableBlocks.hpp"
#include "blue/Context.hpp"
#include "imgui/imgui.h"

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
