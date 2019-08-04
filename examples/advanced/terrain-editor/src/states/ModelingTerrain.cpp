#include "states/ModelingTerrain.hpp"
#include "imgui/imgui.h"
#include "blue/Context.hpp"
#include "Application.hpp"
#include "Resources.hpp"

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
			const char* listbox_items[] = { "Pine tree", "Tank", "Hurdle", "Wheat", "Boulder", "Small Boulder", "Grass", "Pylon", "Bush", "Cut tree", "Track", "Bridge" };
			ImGui::ListBox("", reinterpret_cast<int*>(&_model), listbox_items, IM_ARRAYSIZE(listbox_items), 4);
			ImGui::SliderFloat("Model scale", &_model_scale, 0.01f, 5.0f);
			ImGui::End();
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
			switch (_model)
			{
			case(Model::PINE_TREE):
			{
				RenderEntity entity;
				entity.position = { static_cast<float>(x), 0, static_cast<float>(y) };
				entity.shader = Resources::instance().shaders.model_shader;
				entity.vertex_array = Resources::instance().models.pine_tree;
				entity.scale = 0.294f;
				entity.rotation = { 0, 0, 0, 0 };
				entity.environment = Application::instance().map_environment.environment;

				entity.id = blue::Context::renderer().add(entity);
				break;
			}
			case(Model::TANK):
			{
				RenderEntity entity;
				entity.position = { static_cast<float>(x), 0, static_cast<float>(y) };
				entity.shader = Resources::instance().shaders.model_shader;
				entity.vertex_array = Resources::instance().models.tank;
				entity.scale = _model_scale;
				entity.rotation = { 0, 0, 0, 0 };
				entity.environment = Application::instance().map_environment.environment;

				entity.id = blue::Context::renderer().add(entity);
				break;
			}
			case(Model::HURDLE):
			{
				RenderEntity entity;
				entity.position = { static_cast<float>(x), 0, static_cast<float>(y) };
				entity.shader = Resources::instance().shaders.model_shader;
				entity.vertex_array = Resources::instance().models.hurdle;
				entity.scale = 0.139f;
				entity.rotation = { 0, 0, 0, 0 };
				entity.environment = Application::instance().map_environment.environment;

				entity.id = blue::Context::renderer().add(entity);
				break;
			}
			case(Model::WHEAT):
			{
				RenderEntity entity;
				entity.position = { static_cast<float>(x), 0, static_cast<float>(y) };
				entity.shader = Resources::instance().shaders.model_shader;
				entity.vertex_array = Resources::instance().models.wheat;
				entity.scale = _model_scale;
				entity.rotation = { 0, 0, 0, 0 };
				entity.environment = Application::instance().map_environment.environment;

				entity.id = blue::Context::renderer().add(entity);
				break;
			}
			case(Model::BOULDER):
			{
				RenderEntity entity;
				entity.position = { static_cast<float>(x), 0, static_cast<float>(y) };
				entity.shader = Resources::instance().shaders.model_shader;
				entity.vertex_array = Resources::instance().models.boulder;
				entity.scale = 0.372f;
				entity.rotation = { 0, 0, 0, 0 };
				entity.environment = Application::instance().map_environment.environment;

				entity.id = blue::Context::renderer().add(entity);
				break;
			}
			case(Model::SMALL_BOULDER):
			{
				RenderEntity entity;
				entity.position = { static_cast<float>(x), 0, static_cast<float>(y) };
				entity.shader = Resources::instance().shaders.model_shader;
				entity.vertex_array = Resources::instance().models.small_boulder;
				entity.scale = 0.200f;
				entity.rotation = { 0, 0, 0, 0 };
				entity.environment = Application::instance().map_environment.environment;

				entity.id = blue::Context::renderer().add(entity);
				break;
			}
			case(Model::GRASS):
			{
				RenderEntity entity;
				entity.position = { static_cast<float>(x), 0, static_cast<float>(y) };
				entity.shader = Resources::instance().shaders.model_shader;
				entity.vertex_array = Resources::instance().models.grass;
				entity.scale = 0.185f;
				entity.rotation = { 0, 0, 0, 0 };
				entity.environment = Application::instance().map_environment.environment;

				entity.id = blue::Context::renderer().add(entity);
				break;
			}
			case(Model::PYLON):
			{
				RenderEntity entity;
				entity.position = { static_cast<float>(x), 0, static_cast<float>(y) };
				entity.shader = Resources::instance().shaders.model_shader;
				entity.vertex_array = Resources::instance().models.pylon;
				entity.scale = 0.25f;
				entity.rotation = { 0, 0, 0, 0 };
				entity.environment = Application::instance().map_environment.environment;

				entity.id = blue::Context::renderer().add(entity);
				break;
			}
			case(Model::BUSH):
			{
				RenderEntity entity;
				entity.position = { static_cast<float>(x), 0, static_cast<float>(y) };
				entity.shader = Resources::instance().shaders.model_shader;
				entity.vertex_array = Resources::instance().models.bush;
				entity.scale = 0.55f;
				entity.rotation = { 0, 0, 0, 0 };
				entity.environment = Application::instance().map_environment.environment;

				entity.id = blue::Context::renderer().add(entity);
				break;
			}
			case(Model::CUT_TREE):
			{
				RenderEntity entity;
				entity.position = { static_cast<float>(x), 0, static_cast<float>(y) };
				entity.shader = Resources::instance().shaders.model_shader;
				entity.vertex_array = Resources::instance().models.cut_tree;
				entity.scale = 0.180f;
				entity.rotation = { 0, 0, 0, 0 };
				entity.environment = Application::instance().map_environment.environment;

				entity.id = blue::Context::renderer().add(entity);
				break;
			}
			case(Model::TRACK):
			{
				RenderEntity entity;
				entity.position = { static_cast<float>(x), 0, static_cast<float>(y) };
				entity.shader = Resources::instance().shaders.model_shader;
				entity.vertex_array = Resources::instance().models.track;
				entity.scale = 0.180f;
				entity.rotation = { 0, 0, 0, 0 };
				entity.environment = Application::instance().map_environment.environment;

				entity.id = blue::Context::renderer().add(entity);
				break;
			}
			case(Model::BRIDGE):
			{
				RenderEntity entity;
				entity.position = { static_cast<float>(x), 0, static_cast<float>(y) };
				entity.shader = Resources::instance().shaders.model_shader;
				entity.vertex_array = Resources::instance().models.bridge;
				entity.scale = 0.35f;
				entity.rotation = { 0, 0, 0, 0 };
				entity.environment = Application::instance().map_environment.environment;

				entity.id = blue::Context::renderer().add(entity);
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
	Application::instance().get_map().upload_decoration_vertices();
	// Run intersection test job
	job.start();
}
