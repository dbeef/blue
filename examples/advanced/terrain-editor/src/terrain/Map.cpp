#include "terrain/Map.hpp"
#include "blue/Context.hpp"
#include "blue/camera/PerspectiveCamera.hpp"
#include "blue/ShaderUtils.h"

const float CAMERA_SPEED = 0.25f;

void Map::upload_clickable_vertices()
{
	std::uint32_t vertex_counter = 0;
	std::uint32_t offset = 0;

	for (std::size_t x = 0; x < CHUNK_DIMENSION; x++)
	{
		for (std::size_t y = 0; y < CHUNK_DIMENSION; y++)
		{
			if (!tiles[x][y].clickable) continue;
			for (const auto& vertex : Tile::get_vertices_translated(static_cast<float>(x), static_cast<float>(y)))
			{
				clickable_vertices.push_back(vertex);
			}
			for (const auto& index : Tile::get_indices(offset ))
			{
				clickable_indices.push_back(index);
			}
			vertex_counter += 6;
			offset++;
		}
	}

	auto attributes = Tile::get_attributes();

	auto vertex_array_future = blue::Context::gpu_system().submit(CreateMeshEntity{ clickable_vertices, clickable_indices, attributes, vertex_counter });
	vertex_array_future.wait();
	clickable_vertices_id = vertex_array_future.get();

	auto compile_shader_entity = ShaderUtils::make_entity("resources/Triangle.vertex.glsl", "resources/Triangle.fragment.glsl");
	auto shader_future = blue::Context::gpu_system().submit(compile_shader_entity);
	shader_future.wait();
	auto shader = shader_future.get();

	// Create environment

	auto environment_future = blue::Context::gpu_system().submit(CreateEnvironmentEntity{});
	environment_future.wait();
	environment = environment_future.get();

	// Upload camera's matrices

	blue::Context::gpu_system().submit(UpdateEnvironmentEntity_Projection{ environment, camera.get_projection() });
	blue::Context::gpu_system().submit(UpdateEnvironmentEntity_View{ environment, camera.get_view() });

	// Submit render command consisting of compiled shader, uploaded mesh and following geometry properties:

	RenderEntity entity; 
	entity.position = { 0, 0, 0};
	entity.shader = shader;
	entity.vertex_array = clickable_vertices_id;
	entity.scale = 1.0f;
	entity.rotation = glm::identity<glm::quat>();
	entity.environment = environment;
	entity.texture = 0;

	RenderEntityId id = blue::Context::renderer().add(entity);

	auto w_callback = [this]()
	{
		camera.go_forward(CAMERA_SPEED);
		blue::Context::gpu_system().submit(UpdateEnvironmentEntity_View{ environment, camera.get_view() });
	};
	blue::Context::input().registerKeyCallback({ w_callback, SDLK_w, SDL_KEYDOWN });

	auto s_callback = [this]()
	{
		camera.go_backward(CAMERA_SPEED);
		blue::Context::gpu_system().submit(UpdateEnvironmentEntity_View{ environment, camera.get_view() });
	};
	blue::Context::input().registerKeyCallback({ s_callback, SDLK_s, SDL_KEYDOWN });

	auto a_callback = [this]()
	{
		camera.go_left(CAMERA_SPEED);
		blue::Context::gpu_system().submit(UpdateEnvironmentEntity_View{ environment, camera.get_view() });
	};
	blue::Context::input().registerKeyCallback({ a_callback, SDLK_a, SDL_KEYDOWN });

	auto d_callback = [this]()
	{
		camera.go_right(CAMERA_SPEED);
		blue::Context::gpu_system().submit(UpdateEnvironmentEntity_View{ environment, camera.get_view() });
	};
	blue::Context::input().registerKeyCallback({ d_callback, SDLK_d, SDL_KEYDOWN });

	auto mouse_move_callback = [this](double xpos, double ypos)
	{
		camera.mouse_rotation(xpos, ypos);
		blue::Context::gpu_system().submit(UpdateEnvironmentEntity_View{ environment, camera.get_view() });
	};

	blue::Context::input().registerMouseMoveCallback(mouse_move_callback);
}

void Map::upload_decoration_vertices()
{
}
