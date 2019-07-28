#include "states/ModelingTerrain.hpp"
#include "imgui/imgui.h"
#include "blue/Context.hpp"

ModelingTerrain::ModelingTerrain()
{
	_window = blue::Context::renderer().add([this]() {
		ImGui::Begin("Blue: Terrain editor [Modeling terrain]");

		if (blue::Context::window().is_cursor_attached())
			ImGui::TextColored(ImVec4(1, 0, 0, 1), "Cursor attached, press middle mouse button to detach.");
		else
			ImGui::TextColored(ImVec4(0, 1, 1, 1), "Cursor detached, press middle mouse button to attach.");

		ImGui::End();
		});
}

ModelingTerrain::~ModelingTerrain()
{
}

std::shared_ptr<BaseState> ModelingTerrain::update()
{
	return nullptr;
}

void ModelingTerrain::on_entry()
{
}
