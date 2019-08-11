//
// Created by dbeef on 8/5/19.
//

#include <terrain/Flora.hpp>
#include <algorithm>
#include <blue/Context.hpp>
#include <Resources.hpp>
#include <fstream>
#include <blue/ResourcesPath.h>

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

        add_entry(model, position, glm::eulerAngles(rotation));

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

    for (const auto &entry : temp)
    {
        if (entry.model != current_model)
        {
            if(!current_instances.empty())
            {
                add_instanced_rendering_entry(current_model, current_instances);
            }

            current_model = entry.model;
            current_instances = {};
        }

        current_instances.push_back(entry.entity.position.x);
        current_instances.push_back(entry.entity.position.y);
        current_instances.push_back(entry.entity.position.z);

        const glm::mat4 RotationMatrix = glm::toMat4(entry.entity.rotation);

        for (std::size_t index = 0; index < 16; index++)
        {
            current_instances.push_back(glm::value_ptr(RotationMatrix)[index]);
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

    switch (model)
    {
        case (Model::PINE_TREE):
        {
            entity.vertex_array = Resources::instance().models.pine_tree;
            entity.scale = 0.294f;
            break;
        }
        case (Model::HURDLE):
        {
            entity.vertex_array = Resources::instance().models.hurdle;
            entity.scale = 0.139f;
            break;
        }
        case (Model::WHEAT):
        {
            entity.vertex_array = Resources::instance().models.wheat;
            entity.scale = 1.0;
            break;
        }
        case (Model::BOULDER):
        {
            entity.vertex_array = Resources::instance().models.boulder;
            entity.scale = 0.372f;
            break;
        }
        case (Model::SMALL_BOULDER):
        {
            entity.vertex_array = Resources::instance().models.small_boulder;
            entity.scale = 0.200f;
            break;
        }
        case (Model::GRASS):
        {
            entity.vertex_array = Resources::instance().models.grass;
            entity.scale = 0.185f;
            break;
        }
        case (Model::PYLON):
        {
            entity.vertex_array = Resources::instance().models.pylon;
            entity.scale = 0.25f;
            break;
        }
        case (Model::BUSH):
        {
            entity.vertex_array = Resources::instance().models.bush;
            entity.scale = 0.55f;
            break;
        }
        case (Model::CUT_TREE):
        {
            entity.vertex_array = Resources::instance().models.cut_tree;
            entity.scale = 0.180f;
            break;
        }
        case (Model::TRACK):
        {
            entity.vertex_array = Resources::instance().models.track;
            entity.scale = 0.180f;
            break;
        }
        case (Model::BRIDGE):
        {
            entity.vertex_array = Resources::instance().models.bridge;
            entity.scale = 0.35f;
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

RenderEntity Flora::add_instanced_rendering_entry(Model model, const Instances& instances)
{
    RenderEntity entity;
    const auto &environment = Resources::instance().map_environment.environment;

    entity.shader = Resources::instance().shaders.model_shader;
    entity.environment = environment;

    switch (model)
    {
        case (Model::PINE_TREE):
        {
            entity.vertex_array = Resources::instance().models.pine_tree;
            entity.scale = 0.294f;
            break;
        }
        case (Model::HURDLE):
        {
            entity.vertex_array = Resources::instance().models.hurdle;
            entity.scale = 0.139f;
            break;
        }
        case (Model::WHEAT):
        {
            entity.vertex_array = Resources::instance().models.wheat;
            entity.scale = 1.0;
            break;
        }
        case (Model::BOULDER):
        {
            entity.vertex_array = Resources::instance().models.boulder;
            entity.scale = 0.372f;
            break;
        }
        case (Model::SMALL_BOULDER):
        {
            entity.vertex_array = Resources::instance().models.small_boulder;
            entity.scale = 0.200f;
            break;
        }
        case (Model::GRASS):
        {
            entity.vertex_array = Resources::instance().models.grass;
            entity.scale = 0.185f;
            break;
        }
        case (Model::PYLON):
        {
            entity.vertex_array = Resources::instance().models.pylon;
            entity.scale = 0.25f;
            break;
        }
        case (Model::BUSH):
        {
            entity.vertex_array = Resources::instance().models.bush;
            entity.scale = 0.55f;
            break;
        }
        case (Model::CUT_TREE):
        {
            entity.vertex_array = Resources::instance().models.cut_tree;
            entity.scale = 0.180f;
            break;
        }
        case (Model::TRACK):
        {
            entity.vertex_array = Resources::instance().models.track;
            entity.scale = 0.180f;
            break;
        }
        case (Model::BRIDGE):
        {
            entity.vertex_array = Resources::instance().models.bridge;
            entity.scale = 0.35f;
            break;
        }
        default:
        {
            blue::Context::logger().error("Failed to match model! Value: {}", static_cast<int>(model));
        }
    }


//    entity.id = blue::Context::renderer().add(entity);

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
