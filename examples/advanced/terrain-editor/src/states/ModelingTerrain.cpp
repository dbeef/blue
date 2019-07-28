#include "states/ModelingTerrain.hpp"
#include "imgui/imgui.h"
#include "blue/Context.hpp"
#include "Application.hpp"

ModelingTerrain::ModelingTerrain()
{
	_window = blue::Context::renderer().add([this]() {
		ImGui::Begin("Blue: Terrain editor [Modeling terrain]");

		if (blue::Context::window().is_cursor_attached())
			ImGui::TextColored(ImVec4(1, 0, 0, 1), "Cursor attached, press middle mouse button to detach.");
		else
			ImGui::TextColored(ImVec4(0, 1, 1, 1), "Cursor detached, press middle mouse button to attach.");

		ImGui::RadioButton("Elevation", reinterpret_cast<int*>(&_mode), 0);
		ImGui::SameLine();
		ImGui::RadioButton("Vertex paint", reinterpret_cast<int*>(&_mode), 1);

		ImGui::SliderFloat("Radius", &_radius, 0.01f, 32.0f);

		ImGui::End();

		ImGui::Begin("Vertex paint");
		ImGui::ColorPicker3("", _paint);

		ImGui::End();
		});
}

ModelingTerrain::~ModelingTerrain()
{
	blue::Context::renderer().remove_imgui_entity({ _window });
	job.shutdown();
	Application::instance().get_map().dispose_current_map_on_gpu(); // TODO: Also remove vertex arrays
}

std::shared_ptr<BaseState> ModelingTerrain::update()
{
	if (Application::instance().input.intersection.load())
	{
		const auto& x = Application::instance().input.intersection_point_x;
		const auto& y = Application::instance().input.intersection_point_y;

		if (_mode == Mode::ELEVATION)
		{
			// Ascend points in radius R from xy point:
			if (Application::instance().input.clicked_button.load() == SDL_BUTTON_LEFT)
			{
				Application::instance().get_map().elevate_points(x, y, _radius, 0.25f);
			}
			else
			{
				Application::instance().get_map().elevate_points(x, y, _radius, -0.25f);
			}
		}
		else if (_mode == Mode::VERTEX_PAINT)
		{
			// Color points in radius R from xy point:
			float R = 5.25f;
			Application::instance().get_map().color_points(x, y, _radius, {_paint[0], _paint[1], _paint[2]});
		}

		// Dispose and reupload:
		Application::instance().get_map().dispose_current_decoration_on_gpus();
		Application::instance().get_map().upload_decoration();

		Application::instance().input.intersection.store(false);
	}

	return nullptr;
}

void ModelingTerrain::on_entry()
{
	// Create map
	Application::instance().get_map().upload_clickable_vertices();
	Application::instance().get_map().upload_decoration_vertices();
	// Run intersection test job
	job.start();
}
