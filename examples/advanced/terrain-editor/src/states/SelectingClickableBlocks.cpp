#include "states/SelectingClickableBlocks.hpp"
#include "states/ModelingTerrain.hpp"
#include "Application.hpp"
#include "blue/Context.hpp"
#include "imgui/imgui.h"

#include "terrain/Map.hpp"

SelectingClickableBlocks::SelectingClickableBlocks()
{
	_blocks_window = blue::Context::renderer().add([this]() {

		ImGui::Begin("Blue: Terrain editor [Selecting clickable blocks / Coloring]");

		if (blue::Context::window().is_cursor_attached())
			ImGui::TextColored(ImVec4(1, 0, 0, 1), "Cursor attached, press middle mouse button to detach.");
		else
			ImGui::TextColored(ImVec4(0, 1, 1, 1), "Cursor detached, press middle mouse button to attach.");

		bool new_level = ImGui::Button("I'm done, take me to the modeling step");
		if (new_level)
		{
			_new_level.store(true);
		}

		ImGui::RadioButton("Coloring", reinterpret_cast<int*>(&_mode), 0);
		ImGui::SameLine();
		ImGui::RadioButton("Selecting clickable blocks", reinterpret_cast<int*>(&_mode), 1);
		ImGui::SameLine();

		ImGui::End();

		if (_mode == Mode::COLOURING)
		{
			ImGui::Begin("Tile paint");
			ImGui::ColorPicker3("Paint 1", _paint_1);
			ImGui::ColorPicker3("Paint 2", _paint_2);
			bool invert_colors = ImGui::Button("Invert");
			if (invert_colors)
			{
				float temp[3];
				std::memcpy(&temp, _paint_1, 3 * sizeof(float));
				std::memcpy(&_paint_1, _paint_2, 3 * sizeof(float));
				std::memcpy(&_paint_2, temp, 3 * sizeof(float));
			}
			ImGui::End();

			ImGui::Begin("Interpolation direction");
			ImGui::RadioButton("Horizontal left-right", reinterpret_cast<int*>(&_direction), 0);
			ImGui::SameLine();
			ImGui::RadioButton("Vertical up-down", reinterpret_cast<int*>(&_direction), 1);
			ImGui::SameLine();
			ImGui::RadioButton("C left-up", reinterpret_cast<int*>(&_direction), 2);
			ImGui::SameLine();
			ImGui::RadioButton("C right-down", reinterpret_cast<int*>(&_direction), 3);
			ImGui::SameLine();
			ImGui::RadioButton("C left-down", reinterpret_cast<int*>(&_direction), 4);
			ImGui::SameLine();
			ImGui::RadioButton("C right-up", reinterpret_cast<int*>(&_direction), 5);
			ImGui::SameLine();
			ImGui::RadioButton("Full", reinterpret_cast<int*>(&_direction), 6);
			ImGui::SameLine();
			ImGui::End();
		}
		else
		{
		}

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
		const auto& x_tile = Application::instance().input.intersection_tile_x;
		const auto& y_tile = Application::instance().input.intersection_tile_y;

		if (_mode == Mode::CLICKABLE_BLOCKS)
		{
			Application::instance().get_map().toggle_tile(x_tile, y_tile);
			Application::instance().get_map().dispose_current_map_on_gpu(); // TODO: Also remove vertex arrays
			Application::instance().get_map().upload_clickable_vertices();
			Application::instance().get_map().upload_decoration_vertices();
		}
		else if (_mode == Mode::COLOURING)
		{
			if (Application::instance().input.clicked_button == SDL_BUTTON_RIGHT)
			{
				// Pick the color

				auto tile_color = Application::instance().get_map().get_color(x_tile, y_tile);
				_paint_1[0] = tile_color.first[0];
				_paint_1[1] = tile_color.first[1];
				_paint_1[2] = tile_color.first[2];

				_paint_2[0] = tile_color.second[0];
				_paint_2[1] = tile_color.second[1];
				_paint_2[2] = tile_color.second[2];
			}
			else
			{
				// Mark tile's color

				Application::instance().get_map().color_tile(x_tile, y_tile, _direction, { _paint_1[0],_paint_1[1],_paint_1[2] }, { _paint_2[0],_paint_2[1],_paint_2[2] });

				Application::instance().get_map().dispose_current_map_on_gpu(); // TODO: Also remove vertex arrays
				Application::instance().get_map().upload_clickable_vertices();
				Application::instance().get_map().upload_decoration_vertices();
			}
		}

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
