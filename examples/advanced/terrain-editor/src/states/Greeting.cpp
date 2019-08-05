#include "states/Greeting.hpp"
#include "states/SelectingClickableBlocks.hpp"
#include "states/ModelingTerrain.hpp"
#include "blue/Context.hpp"
#include "Application.hpp"
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
		
		if (load_level)
		{
			_load_level.store(true);
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
	else if (_load_level.load())
	{
		Application::instance().get_map().import_from_file("resources/map.bin");
		Application::instance().get_flora().import_from_file("resources/flora.bin");
		return std::make_shared<ModelingTerrain>(true);
	}
	else
	{
		return nullptr;
	}
}

void Greeting::on_entry()
{
}
