#include "states/Greeting.hpp"
#include "states/SelectingClickableBlocks.hpp"
#include "blue/Context.hpp"
#include "imgui/imgui.h"

Greeting::Greeting()
{
	_greeting_window = blue::Context::renderer().add([this]() {
		ImGui::Begin("Blue: Terrain editor [Greeting]");
		bool new_level = ImGui::Button("Create new level");
		bool load_level = ImGui::Button("Load level");

		if (new_level)
		{
			_new_level.store(true);
		}

		ImGui::End();
		});
}

Greeting::~Greeting()
{
	blue::Context::renderer().remove_imgui_entity({ _greeting_window });
}

std::shared_ptr<BaseState> Greeting::update()
{
	if (_new_level.load())
	{
		return std::make_shared<SelectingClickableBlocks>();
	}
	else
	{
		return nullptr;
	}
}

void Greeting::on_entry()
{
}
