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
	float get_default_scale(Resources::Model model)
	{
		switch (model)
		{
		case (Resources::Model::PINE_TREE):
		{
			return 0.294f;
		}
		case (Resources::Model::HURDLE):
		{
			return 0.139f;
		}
		case (Resources::Model::WHEAT):
		{
			return 1.0;
		}
		case (Resources::Model::BOULDER):
		{
			return 0.372f;
		}
		case (Resources::Model::SMALL_BOULDER):
		{
			return 0.200f;
		}
		case (Resources::Model::GRASS):
		{
			return 0.185f;
		}
		case (Resources::Model::PYLON):
		{
			return 0.25f;
		}
		case (Resources::Model::BUSH):
		{
			return 0.55f;
		}
		case (Resources::Model::CUT_TREE):
		{
			return 0.180f;
		}
		case (Resources::Model::TRACK):
		{
			return 0.180f;
		}
		case (Resources::Model::BRIDGE):
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
        const Resources::Model model = *reinterpret_cast<Resources::Model*>(&data[offset]);
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

	Resources::Model current_model{};
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
		const glm::mat4 ModelMatrix = TranslationMatrix * RotationMatrix * ScaleMatrix;

        for (std::size_t index = 0; index < 16; index++)
        {
            current_instances.push_back(glm::value_ptr(ModelMatrix)[index]);
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

RenderEntity Flora::add_entry(Resources::Model model, const glm::vec3 &position, const glm::vec3 &rotation)
{
    RenderEntity entity;
    const auto &environment = Resources::instance().map_environment.environment;

    entity.position = position;
    entity.shader = Resources::instance().shaders.model;
    entity.rotation = glm::quat(rotation);
    entity.environment = environment;
	entity.scale = get_default_scale(model);
    entity.framebuffer.framebuffer = 0;

    switch (model)
    {
        case (Resources::Model::PINE_TREE):
        {
            entity.vertex_array = Resources::instance().models.pine_tree;
            break;
        }
        case (Resources::Model::HURDLE):
        {
            entity.vertex_array = Resources::instance().models.hurdle;
            break;
        }
        case (Resources::Model::WHEAT):
        {
            entity.vertex_array = Resources::instance().models.wheat;
            break;
        }
        case (Resources::Model::BOULDER):
        {
            entity.vertex_array = Resources::instance().models.boulder;
            break;
        }
        case (Resources::Model::SMALL_BOULDER):
        {
            entity.vertex_array = Resources::instance().models.small_boulder;
            break;
        }
        case (Resources::Model::GRASS):
        {
            entity.vertex_array = Resources::instance().models.grass;
            break;
        }
        case (Resources::Model::PYLON):
        {
            entity.vertex_array = Resources::instance().models.pylon;
            break;
        }
        case (Resources::Model::BUSH):
        {
            entity.vertex_array = Resources::instance().models.bush;
            break;
        }
        case (Resources::Model::CUT_TREE):
        {
            entity.vertex_array = Resources::instance().models.cut_tree;
            break;
        }
        case (Resources::Model::TRACK):
        {
            entity.vertex_array = Resources::instance().models.track;
            break;
        }
        case (Resources::Model::BRIDGE):	
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

RenderEntity Flora::add_instanced_rendering_entry(Resources::Model model, const Instances& instances, std::uint32_t instances_count)
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

	Attributes swinging_attributes =
	{
		{ ShaderAttribute::Type::VEC3, ShaderAttribute::Purpose::VERTEX_POSITION, ShaderAttribute::Buffer::VERTEX},
		{ ShaderAttribute::Type::VEC3, ShaderAttribute::Purpose::COLOR, ShaderAttribute::Buffer::VERTEX},
		{ ShaderAttribute::Type::VEC3, ShaderAttribute::Purpose::NORMAL, ShaderAttribute::Buffer::VERTEX},
        { ShaderAttribute::Type::FLOAT, ShaderAttribute::Purpose::NORMALIZED_HEIGHT, ShaderAttribute::Buffer::VERTEX},
        // Attributes from instance buffer:
		// Uploading 4x4 matrix this way, since there's a size limit for single attribute in OpenGL:
		{ ShaderAttribute::Type::VEC4, ShaderAttribute::Purpose::MODEL, ShaderAttribute::Buffer::INSTANCED},
		{ ShaderAttribute::Type::VEC4, ShaderAttribute::Purpose::MODEL, ShaderAttribute::Buffer::INSTANCED},
		{ ShaderAttribute::Type::VEC4, ShaderAttribute::Purpose::MODEL, ShaderAttribute::Buffer::INSTANCED},
		{ ShaderAttribute::Type::VEC4, ShaderAttribute::Purpose::MODEL, ShaderAttribute::Buffer::INSTANCED},
	};

    RenderEntity entity;
    const auto &environment = Resources::instance().map_environment.environment;

    entity.shader = Resources::instance().shaders.model_instanced;
    entity.environment = environment;
	entity.position = {0, 0, 0};
	entity.rotation = glm::identity<glm::quat>();

    switch (model)
    {
        case (Resources::Model::PINE_TREE):
        {
            entity.shader = Resources::instance().shaders.swinging;
            auto instanced_vertex_array = blue::Context::gpu_system().submit(CreateInstancedMeshEntity{ Resources::instance().models.pine_tree, swinging_attributes, instances, instances_count }).get();
            entity.vertex_array = instanced_vertex_array;
			break;
        }
        case (Resources::Model::HURDLE):
        {
            auto instanced_vertex_array = blue::Context::gpu_system().submit(CreateInstancedMeshEntity{ Resources::instance().models.hurdle, attributes, instances, instances_count }).get();
            entity.vertex_array = instanced_vertex_array;
            break;
        }
        case (Resources::Model::WHEAT):
        {
            auto instanced_vertex_array = blue::Context::gpu_system().submit(CreateInstancedMeshEntity{ Resources::instance().models.wheat, attributes, instances, instances_count }).get();
            entity.vertex_array = instanced_vertex_array;
            break;
        }
        case (Resources::Model::BOULDER):
        {
            auto instanced_vertex_array = blue::Context::gpu_system().submit(CreateInstancedMeshEntity{ Resources::instance().models.boulder, attributes, instances, instances_count }).get();
            entity.vertex_array = instanced_vertex_array;
            break;
        }
        case (Resources::Model::SMALL_BOULDER):
        {
            auto instanced_vertex_array = blue::Context::gpu_system().submit(CreateInstancedMeshEntity{ Resources::instance().models.small_boulder, attributes, instances, instances_count }).get();
            entity.vertex_array = instanced_vertex_array;
            break;
        }
        case (Resources::Model::GRASS):
        {
            auto instanced_vertex_array = blue::Context::gpu_system().submit(CreateInstancedMeshEntity{ Resources::instance().models.grass, attributes, instances, instances_count }).get();
            entity.vertex_array = instanced_vertex_array;
            break;
        }
        case (Resources::Model::PYLON):
        {
            auto instanced_vertex_array = blue::Context::gpu_system().submit(CreateInstancedMeshEntity{ Resources::instance().models.pylon, attributes, instances, instances_count }).get();
            entity.vertex_array = instanced_vertex_array;
            break;
        }
        case (Resources::Model::BUSH):
        {
            auto instanced_vertex_array = blue::Context::gpu_system().submit(CreateInstancedMeshEntity{ Resources::instance().models.bush, attributes, instances, instances_count }).get();
            entity.vertex_array = instanced_vertex_array;
            break;
        }
        case (Resources::Model::CUT_TREE):
        {
            auto instanced_vertex_array = blue::Context::gpu_system().submit(CreateInstancedMeshEntity{ Resources::instance().models.cut_tree, attributes, instances, instances_count }).get();
            entity.vertex_array = instanced_vertex_array;
            break;
        }
        case (Resources::Model::TRACK):
        {
            auto instanced_vertex_array = blue::Context::gpu_system().submit(CreateInstancedMeshEntity{ Resources::instance().models.track, attributes, instances, instances_count }).get();
            entity.vertex_array = instanced_vertex_array;
            break;
        }
        case (Resources::Model::BRIDGE):
        {
            auto instanced_vertex_array = blue::Context::gpu_system().submit(CreateInstancedMeshEntity{ Resources::instance().models.bridge, attributes, instances, instances_count }).get();
            entity.vertex_array = instanced_vertex_array;
            break;
        }
        default:
        {
            blue::Context::logger().error("Failed to match model! Value: {}", static_cast<int>(model));
        }
    }

	entity.scale = 1.0f;
	entity.texture = Resources::instance().light_environment.depth.texture;
//    entity.environment = Resources::instance().light_environment.environment;

    entity.id = blue::Context::renderer().add(entity);

    RenderEntity shadow;
    shadow.position = entity.position;
	shadow.shader = Resources::instance().shaders.simple_depth_instanced;
    shadow.vertex_array = entity.vertex_array;
    shadow.scale = entity.scale;
    shadow.rotation = entity.rotation;
    shadow.environment = Resources::instance().light_environment.environment;
    shadow.texture = 0;
    shadow.framebuffer = Resources::instance().light_environment.depth;
    shadow.id = blue::Context::renderer().add(shadow);

    entries.push_back({model, entity});
    entries.push_back({model, shadow});
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
