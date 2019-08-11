//
// Created by dbeef on 8/5/19.
//

#include <terrain/Flora.hpp>
#include <algorithm>
#include <blue/Context.hpp>
#include <Resources.hpp>
#include <fstream>
#include <blue/ResourcesPath.h>

namespace
{
	float get_default_scale(Model model)
	{
		switch (model)
		{
		case (Model::PINE_TREE):
		{
			return 0.294f;
		}
		case (Model::HURDLE):
		{
			return 0.139f;
		}
		case (Model::WHEAT):
		{
			return 1.0;
		}
		case (Model::BOULDER):
		{
			return 0.372f;
		}
		case (Model::SMALL_BOULDER):
		{
			return 0.200f;
		}
		case (Model::GRASS):
		{
			return 0.185f;
		}
		case (Model::PYLON):
		{
			return 0.25f;
		}
		case (Model::BUSH):
		{
			return 0.55f;
		}
		case (Model::CUT_TREE):
		{
			return 0.180f;
		}
		case (Model::TRACK):
		{
			return 0.180f;
		}
		case (Model::BRIDGE):
		{
			return 0.35f;
		}
		default:
		{
			blue::Context::logger().error("Failed to match model! Value: {}", static_cast<int>(model));
			return 1.0f;
		}
		}
	}
}

void Flora::import_from_file(const std::string &filename)
{
    auto path_extended = paths::getResourcesPath() + filename;

    std::vector<char> data;

    SDL_RWops *file = SDL_RWFromFile(path_extended.c_str(), "rb");
    if (file != NULL)
    {
        auto size = file->size(file);
        //Initialize data
        data.resize(size);
        auto read = file->read(file, &data[0], size, 1);
        //Close file handler
        SDL_RWclose(file);
    } else
    {
        blue::Context::logger().error("Error: Unable to open map file. SDL Error: {}", SDL_GetError());
        return;
    }

    if (data.empty())
    {
        blue::Context::logger().error("Error: Empty flora file.");
        return;
    }

    std::uint64_t offset = 0;
    std::uint64_t number_of_entries = *reinterpret_cast<std::uint64_t *>(&data[offset]);
    offset += sizeof(std::uint64_t);

    blue::Context::logger().info("Found {} flora entries.", number_of_entries);

    std::vector<Entry> temp;

    for (std::size_t index = 0; index < number_of_entries; index++)
    {
        const Model model = *reinterpret_cast<Model *>(&data[offset]);
        offset += sizeof(model);

        const glm::vec3 position = *reinterpret_cast<glm::vec3 *>(&data[offset]);
        offset += sizeof(position);

        const glm::quat rotation = *reinterpret_cast<glm::quat *>(&data[offset]);
        offset += sizeof(rotation);

        Entry e;
        e.model = model;
        e.entity.position = position;
        e.entity.rotation = rotation;
        temp.push_back(e);
    }

    std::sort(temp.begin(), temp.end(), [](const Entry &first, const Entry &second)
    { 
        return first.model > second.model;
    });

	Model current_model{};
    Instances current_instances{};
	std::uint32_t instances_count = 0;

    for (std::size_t index = 0; index < temp.size(); index++)
    {
		const auto& entry = temp[index];
		bool last_entry = index == temp.size() - 1;

        if (entry.model != current_model || last_entry)
        {
            if(!current_instances.empty())
            {
                add_instanced_rendering_entry(current_model, current_instances, instances_count);
            }

            current_model = entry.model;
            current_instances = {};
			instances_count = 0;
        }

		instances_count++;

		const glm::mat4 ScaleMatrix = glm::scale(glm::identity<glm::mat4>(), glm::vec3(get_default_scale(entry.model)));
		const glm::mat4 RotationMatrix = glm::toMat4(entry.entity.rotation);
		const glm::mat4 TranslationMatrix = glm::translate(glm::identity<glm::mat4>(), entry.entity.position);
		const glm::mat4 Model = TranslationMatrix * RotationMatrix * ScaleMatrix;

        for (std::size_t index = 0; index < 16; index++)
        {
            current_instances.push_back(glm::value_ptr(Model)[index]);
        }
    }
}

void Flora::export_to_file(const std::string &filename)
{
    std::fstream out(paths::getResourcesPath() + filename, std::fstream::binary | std::fstream::out);

    if (!out.is_open())
    {
        blue::Context::logger().error("Failed to open file: {} for saving map", filename);
        return;
    }

    std::uint64_t size = entries.size();
    out.write(reinterpret_cast<char *>(&size), sizeof(std::uint64_t));

    for (const auto &entry : entries)
    {
        out.write(reinterpret_cast<const char *>(&entry.model), sizeof(entry.model));
        out.write(reinterpret_cast<const char *>(glm::value_ptr(entry.entity.position)), sizeof(glm::vec3));
        out.write(reinterpret_cast<const char *>(glm::value_ptr(entry.entity.rotation)), sizeof(glm::quat));
    }

    out.close();
}

void Flora::remove_entry(const RenderEntity &render_entity)
{
    auto iterator = std::remove_if(entries.begin(), entries.end(), [render_entity](const Entry &entry)
                                   {
                                       return entry.entity.id == render_entity.id;
                                   }
    );
}

RenderEntity Flora::add_entry(Model model, const glm::vec3 &position, const glm::vec3 &rotation)
{
    RenderEntity entity;
    const auto &environment = Resources::instance().map_environment.environment;

    entity.position = position;
    entity.shader = Resources::instance().shaders.model_shader;
    entity.rotation = glm::quat(rotation);
    entity.environment = environment;
	entity.scale = get_default_scale(model);

    switch (model)
    {
        case (Model::PINE_TREE):
        {
            entity.vertex_array = Resources::instance().models.pine_tree;
            break;
        }
        case (Model::HURDLE):
        {
            entity.vertex_array = Resources::instance().models.hurdle;
            break;
        }
        case (Model::WHEAT):
        {
            entity.vertex_array = Resources::instance().models.wheat;
            break;
        }
        case (Model::BOULDER):
        {
            entity.vertex_array = Resources::instance().models.boulder;
            break;
        }
        case (Model::SMALL_BOULDER):
        {
            entity.vertex_array = Resources::instance().models.small_boulder;
            break;
        }
        case (Model::GRASS):
        {
            entity.vertex_array = Resources::instance().models.grass;
            break;
        }
        case (Model::PYLON):
        {
            entity.vertex_array = Resources::instance().models.pylon;
            break;
        }
        case (Model::BUSH):
        {
            entity.vertex_array = Resources::instance().models.bush;
            break;
        }
        case (Model::CUT_TREE):
        {
            entity.vertex_array = Resources::instance().models.cut_tree;
            break;
        }
        case (Model::TRACK):
        {
            entity.vertex_array = Resources::instance().models.track;
            break;
        }
        case (Model::BRIDGE):
        {
            entity.vertex_array = Resources::instance().models.bridge;
            break;
        }
        default:
        {
            blue::Context::logger().error("Failed to match model! Value: {}", static_cast<int>(model));
        }
    }

    entity.id = blue::Context::renderer().add(entity);

    entries.push_back({model, entity});
    return entity;
}

RenderEntity Flora::add_instanced_rendering_entry(Model model, const Instances& instances, std::uint32_t instances_count)
{
	Attributes attributes =
	{
		{ ShaderAttribute::Type::VEC3, ShaderAttribute::Purpose::VERTEX_POSITION, ShaderAttribute::Buffer::VERTEX},
		{ ShaderAttribute::Type::VEC3, ShaderAttribute::Purpose::COLOR, ShaderAttribute::Buffer::VERTEX},
		{ ShaderAttribute::Type::VEC3, ShaderAttribute::Purpose::NORMAL, ShaderAttribute::Buffer::VERTEX},
		// Attributes from instance buffer:
		// Uploading 4x4 matrix this way, since there's a size limit for single attribute in OpenGL:
		{ ShaderAttribute::Type::VEC4, ShaderAttribute::Purpose::MODEL, ShaderAttribute::Buffer::INSTANCED},
		{ ShaderAttribute::Type::VEC4, ShaderAttribute::Purpose::MODEL, ShaderAttribute::Buffer::INSTANCED},
		{ ShaderAttribute::Type::VEC4, ShaderAttribute::Purpose::MODEL, ShaderAttribute::Buffer::INSTANCED},
		{ ShaderAttribute::Type::VEC4, ShaderAttribute::Purpose::MODEL, ShaderAttribute::Buffer::INSTANCED},
	};

    RenderEntity entity;
    const auto &environment = Resources::instance().map_environment.environment;

    entity.shader = Resources::instance().shaders.model_shader;
    entity.environment = environment;
	entity.position = {0, 0, 0};
	entity.rotation = glm::identity<glm::quat>();

    switch (model)
    {
        case (Model::PINE_TREE):
        {
            entity.vertex_array = Resources::instance().models.pine_tree;
			break;
        }
        case (Model::HURDLE):
        {
            entity.vertex_array = Resources::instance().models.hurdle;
            break;
        }
        case (Model::WHEAT):
        {
            entity.vertex_array = Resources::instance().models.wheat;
            break;
        }
        case (Model::BOULDER):
        {
            entity.vertex_array = Resources::instance().models.boulder;
            break;
        }
        case (Model::SMALL_BOULDER):
        {
            entity.vertex_array = Resources::instance().models.small_boulder;
            break;
        }
        case (Model::GRASS):
        {
            entity.vertex_array = Resources::instance().models.grass;
            break;
        }
        case (Model::PYLON):
        {
            entity.vertex_array = Resources::instance().models.pylon;
            break;
        }
        case (Model::BUSH):
        {
            entity.vertex_array = Resources::instance().models.bush;
            break;
        }
        case (Model::CUT_TREE):
        {
            entity.vertex_array = Resources::instance().models.cut_tree;
            break;
        }
        case (Model::TRACK):
        {
            entity.vertex_array = Resources::instance().models.track;
            break;
        }
        case (Model::BRIDGE):
        {
            entity.vertex_array = Resources::instance().models.bridge;
            break;
        }
        default:
        {
            blue::Context::logger().error("Failed to match model! Value: {}", static_cast<int>(model));
        }
    }
	
	auto instanced_vertex_array = blue::Context::gpu_system().submit(CreateInstancedMeshEntity{ entity.vertex_array, attributes, instances, instances_count }).get();
	entity.vertex_array = instanced_vertex_array;
	entity.scale = 1.0f;
    entity.id = blue::Context::renderer().add(entity);

    entries.push_back({model, entity});
    return entity;
}

void Flora::update_entry(const RenderEntity &render_entity)
{
    auto iter = std::find_if(entries.begin(), entries.end(), [render_entity](const Entry &e)
    { return e.entity.id == render_entity.id; });

    if (iter != entries.end())
    {
        iter->entity.rotation = render_entity.rotation;
        iter->entity.position = render_entity.position;
    }
}
