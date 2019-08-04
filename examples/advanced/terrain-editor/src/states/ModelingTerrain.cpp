#include "states/ModelingTerrain.hpp"
#include "imgui/imgui.h"
#include "blue/Context.hpp"
#include "Application.hpp"
#include "Resources.hpp"

ModelingTerrain::ModelingTerrain(const bool map_imported) : _map_imported(map_imported)
{
	_window = blue::Context::renderer().add([this]() {
		ImGui::Begin("Blue: Terrain editor [Modeling terrain]");

		if (blue::Context::window().is_cursor_attached())
			ImGui::TextColored(ImVec4(1, 0, 0, 1), "Cursor attached, press middle mouse button to detach.");
		else
			ImGui::TextColored(ImVec4(0, 1, 1, 1), "Cursor detached, press middle mouse button to attach.");

		bool export_map = ImGui::Button("Save to file");
		if (export_map)
		{
			Application::instance().get_map().export_to_file("map.bin");
		}

		ImGui::RadioButton("Elevation", reinterpret_cast<int*>(&_mode), 0);
		ImGui::SameLine();
		ImGui::RadioButton("Vertex paint", reinterpret_cast<int*>(&_mode), 1);
		ImGui::SameLine();
		ImGui::RadioButton("Adding models", reinterpret_cast<int*>(&_mode), 2);
		ImGui::SameLine();
		ImGui::RadioButton("Vertex paint shuffle", reinterpret_cast<int*>(&_mode), 3);

		if (_mode == Mode::VERTEX_PAINT || _mode == Mode::ELEVATION || _mode == Mode::VERTEX_PAINT_SHUFFLE)
		{
			ImGui::SliderFloat("Radius", &_radius, 0.01f, 32.0f);
		}

		ImGui::End();

		if (_mode == Mode::VERTEX_PAINT)
		{
			ImGui::Begin("Vertex paint");
			ImGui::ColorPicker3("", _paint);
			ImGui::End();
		}

		else if (_mode == Mode::ADDING_MODELS)
		{
			ImGui::Begin("Models list");
			const char* listbox_items[] = { "Pine tree", "Hurdle", "Wheat", "Boulder", "Small Boulder", "Grass", "Pylon", "Bush", "Cut tree", "Track", "Bridge" };
			ImGui::ListBox("", reinterpret_cast<int*>(&_model), listbox_items, IM_ARRAYSIZE(listbox_items), 4);
			ImGui::SliderFloat("Model scale", &_model_scale, 0.01f, 5.0f);
			ImGui::End();

			if (last_entity.entity.id)
			{
				ImGui::Begin("Last entity parameters:");

				ImGui::SliderFloat("Rotation X", &last_entity.euler.x, 0.00f, 3.0f);
				ImGui::SliderFloat("Rotation Y", &last_entity.euler.y, 0.00f, 3.0f);
				ImGui::SliderFloat("Rotation Z", &last_entity.euler.z, 0.00f, 3.0f);

				ImGui::InputFloat("Position X", &last_entity.position.x, 0.01f, 32.0f, "%.3f");
				ImGui::InputFloat("Position Y", &last_entity.position.y, 0.01f, 32.0f, "%.3f");
				ImGui::InputFloat("Position Z", &last_entity.position.z, 0.01f, 32.0f, "%.3f");

				bool updated = ImGui::Button("Update");

				if (updated)
				{
					last_entity.entity.rotation = glm::quat(last_entity.euler);
					last_entity.entity.position = last_entity.position;
					updated_model.store(true);
				}

				ImGui::End();
			}
		}

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
	if (updated_model)
	{
		blue::Context::renderer().update(last_entity.entity);
		updated_model.store(false);
	}

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
			Application::instance().get_map().color_points(x, y, _radius, { _paint[0], _paint[1], _paint[2] });
		}
		else if (_mode == Mode::VERTEX_PAINT_SHUFFLE)
		{
			// Color points in radius R from xy point:
			float R = 5.25f;
			Application::instance().get_map().shuffle_color_points(x, y, _radius);
		}
		else if (_mode == Mode::ADDING_MODELS)
		{
			auto& map_environment = Resources::instance().map_environment;

			switch (_model)
			{
			case(Model::PINE_TREE):
			{
				last_entity.entity.position = { static_cast<float>(x), 0, static_cast<float>(y) };
				last_entity.entity.shader = Resources::instance().shaders.model_shader;
				last_entity.entity.vertex_array = Resources::instance().models.pine_tree;
				last_entity.entity.scale = 0.294f;
				last_entity.entity.rotation = { 0, 0, 0, 0 };
				last_entity.entity.environment = map_environment.environment;

				last_entity.entity.id = blue::Context::renderer().add(last_entity.entity);

				last_entity.position = { static_cast<float>(x), 0, static_cast<float>(y) };

				break;
			}
			case(Model::HURDLE):
			{
				last_entity.entity.position = { static_cast<float>(x), 0, static_cast<float>(y) };
				last_entity.entity.shader = Resources::instance().shaders.model_shader;
				last_entity.entity.vertex_array = Resources::instance().models.hurdle;
				last_entity.entity.scale = 0.139f;
				last_entity.entity.rotation = { 0, 0, 0, 0 };
				last_entity.entity.environment = map_environment.environment;

				last_entity.entity.id = blue::Context::renderer().add(last_entity.entity);

				last_entity.position = { static_cast<float>(x), 0, static_cast<float>(y) };
				break;
			}
			case(Model::WHEAT):
			{
				last_entity.entity.position = { static_cast<float>(x), 0, static_cast<float>(y) };
				last_entity.entity.shader = Resources::instance().shaders.model_shader;
				last_entity.entity.vertex_array = Resources::instance().models.wheat;
				last_entity.entity.scale = _model_scale;
				last_entity.entity.rotation = { 0, 0, 0, 0 };
				last_entity.entity.environment = map_environment.environment;

				last_entity.entity.id = blue::Context::renderer().add(last_entity.entity);

				last_entity.position = { static_cast<float>(x), 0, static_cast<float>(y) };
				break;
			}
			case(Model::BOULDER):
			{
				last_entity.entity.position = { static_cast<float>(x), 0, static_cast<float>(y) };
				last_entity.entity.shader = Resources::instance().shaders.model_shader;
				last_entity.entity.vertex_array = Resources::instance().models.boulder;
				last_entity.entity.scale = 0.372f;
				last_entity.entity.rotation = { 0, 0, 0, 0 };
				last_entity.entity.environment = map_environment.environment;

				last_entity.entity.id = blue::Context::renderer().add(last_entity.entity);

				last_entity.position = { static_cast<float>(x), 0, static_cast<float>(y) };
				break;
			}
			case(Model::SMALL_BOULDER):
			{
				last_entity.entity.position = { static_cast<float>(x), 0, static_cast<float>(y) };
				last_entity.entity.shader = Resources::instance().shaders.model_shader;
				last_entity.entity.vertex_array = Resources::instance().models.small_boulder;
				last_entity.entity.scale = 0.200f;
				last_entity.entity.rotation = { 0, 0, 0, 0 };
				last_entity.entity.environment = map_environment.environment;

				last_entity.entity.id = blue::Context::renderer().add(last_entity.entity);

				last_entity.position = { static_cast<float>(x), 0, static_cast<float>(y) };
				break;
			}
			case(Model::GRASS):
			{
				last_entity.entity.position = { static_cast<float>(x), 0, static_cast<float>(y) };
				last_entity.entity.shader = Resources::instance().shaders.model_shader;
				last_entity.entity.vertex_array = Resources::instance().models.grass;
				last_entity.entity.scale = 0.185f;
				last_entity.entity.rotation = { 0, 0, 0, 0 };
				last_entity.entity.environment = map_environment.environment;

				last_entity.entity.id = blue::Context::renderer().add(last_entity.entity);

				last_entity.position = { static_cast<float>(x), 0, static_cast<float>(y) };
				break;
			}
			case(Model::PYLON):
			{
				last_entity.entity.position = { static_cast<float>(x), 0, static_cast<float>(y) };
				last_entity.entity.shader = Resources::instance().shaders.model_shader;
				last_entity.entity.vertex_array = Resources::instance().models.pylon;
				last_entity.entity.scale = 0.25f;
				last_entity.entity.rotation = { 0, 0, 0, 0 };
				last_entity.entity.environment = map_environment.environment;

				last_entity.entity.id = blue::Context::renderer().add(last_entity.entity);

				last_entity.position = { static_cast<float>(x), 0, static_cast<float>(y) };
				break;
			}
			case(Model::BUSH):
			{
				last_entity.entity.position = { static_cast<float>(x), 0, static_cast<float>(y) };
				last_entity.entity.shader = Resources::instance().shaders.model_shader;
				last_entity.entity.vertex_array = Resources::instance().models.bush;
				last_entity.entity.scale = 0.55f;
				last_entity.entity.rotation = { 0, 0, 0, 0 };
				last_entity.entity.environment = map_environment.environment;

				last_entity.entity.id = blue::Context::renderer().add(last_entity.entity);

				last_entity.position = { static_cast<float>(x), 0, static_cast<float>(y) };
				break;
			}
			case(Model::CUT_TREE):
			{
				last_entity.entity.position = { static_cast<float>(x), 0, static_cast<float>(y) };
				last_entity.entity.shader = Resources::instance().shaders.model_shader;
				last_entity.entity.vertex_array = Resources::instance().models.cut_tree;
				last_entity.entity.scale = 0.180f;
				last_entity.entity.rotation = { 0, 0, 0, 0 };
				last_entity.entity.environment = map_environment.environment;

				last_entity.entity.id = blue::Context::renderer().add(last_entity.entity);

				last_entity.position = { static_cast<float>(x), 0, static_cast<float>(y) };
				break;
			}
			case(Model::TRACK):
			{
				last_entity.entity.position = { static_cast<float>(x), 0, static_cast<float>(y) };
				last_entity.entity.shader = Resources::instance().shaders.model_shader;
				last_entity.entity.vertex_array = Resources::instance().models.track;
				last_entity.entity.scale = 0.180f;
				last_entity.entity.rotation = { 0, 0, 0, 0 };
				last_entity.entity.environment = map_environment.environment;

				last_entity.entity.id = blue::Context::renderer().add(last_entity.entity);

				last_entity.position = { static_cast<float>(x), 0, static_cast<float>(y) };
				break;
			}
			case(Model::BRIDGE):
			{
				last_entity.entity.position = { static_cast<float>(x), 0, static_cast<float>(y) };
				last_entity.entity.shader = Resources::instance().shaders.model_shader;
				last_entity.entity.vertex_array = Resources::instance().models.bridge;
				last_entity.entity.scale = 0.35f;
				last_entity.entity.rotation = { 0, 0, 0, 0 };
				last_entity.entity.environment = map_environment.environment;

				last_entity.entity.id = blue::Context::renderer().add(last_entity.entity);

				last_entity.position = { static_cast<float>(x), 0, static_cast<float>(y) };
				break;
			}
			}
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
	if (_map_imported)
	{
		Application::instance().get_map().upload_decoration();
	}
	else
	{
		Application::instance().get_map().upload_decoration_vertices();
	}
	// Run intersection test job
	job.start();
}
