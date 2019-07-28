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
		
		// Ascend points in radius R from xy point:
		
		float R = 0.25f;
		Application::instance().get_map().ascend_points(x, y, R);

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
