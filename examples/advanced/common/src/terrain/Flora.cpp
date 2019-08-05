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

    for (std::size_t index = 0; index < number_of_entries; index++)
    {
        Model model = *reinterpret_cast<Model *>(&data[offset]);
        offset += sizeof(model);

        glm::vec3 position = *reinterpret_cast<glm::vec3 *>(&data[offset]);
        offset += sizeof(position);

        glm::quat rotation = *reinterpret_cast<glm::quat *>(&data[offset]);
        offset += sizeof(rotation);

        add_entry(model, position, glm::eulerAngles(rotation));
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

    switch (model)
    {
        case (Model::PINE_TREE):
        {
            entity.position = position;
            entity.shader = Resources::instance().shaders.model_shader;
            entity.vertex_array = Resources::instance().models.pine_tree;
            entity.scale = 0.294f;
            entity.rotation = glm::quat(rotation);
            entity.environment = environment;
            entity.id = blue::Context::renderer().add(entity);
            break;
        }
        case (Model::HURDLE):
        {
            entity.position = position;
            entity.shader = Resources::instance().shaders.model_shader;
            entity.vertex_array = Resources::instance().models.hurdle;
            entity.scale = 0.139f;
            entity.rotation = glm::quat(rotation);
            entity.environment = environment;
            entity.id = blue::Context::renderer().add(entity);
            break;
        }
        case (Model::WHEAT):
        {
            entity.position = position;
            entity.shader = Resources::instance().shaders.model_shader;
            entity.vertex_array = Resources::instance().models.wheat;
            entity.scale = 1.0;
            entity.rotation = glm::quat(rotation);
            entity.environment = environment;
            entity.id = blue::Context::renderer().add(entity);
            break;
        }
        case (Model::BOULDER):
        {
            entity.position = position;
            entity.shader = Resources::instance().shaders.model_shader;
            entity.vertex_array = Resources::instance().models.boulder;
            entity.scale = 0.372f;
            entity.rotation = glm::quat(rotation);
            entity.environment = environment;
            entity.id = blue::Context::renderer().add(entity);
            break;
        }
        case (Model::SMALL_BOULDER):
        {
            entity.position = position;
            entity.shader = Resources::instance().shaders.model_shader;
            entity.vertex_array = Resources::instance().models.small_boulder;
            entity.scale = 0.200f;
            entity.rotation = glm::quat(rotation);
            entity.environment = environment;
            entity.id = blue::Context::renderer().add(entity);
            break;
        }
        case (Model::GRASS):
        {
            entity.position = position;
            entity.shader = Resources::instance().shaders.model_shader;
            entity.vertex_array = Resources::instance().models.grass;
            entity.scale = 0.185f;
            entity.rotation = glm::quat(rotation);
            entity.environment = environment;
            entity.id = blue::Context::renderer().add(entity);
            break;
        }
        case (Model::PYLON):
        {
            entity.position = position;
            entity.shader = Resources::instance().shaders.model_shader;
            entity.vertex_array = Resources::instance().models.pylon;
            entity.scale = 0.25f;
            entity.rotation = glm::quat(rotation);
            entity.environment = environment;
            entity.id = blue::Context::renderer().add(entity);
            break;
        }
        case (Model::BUSH):
        {
            entity.position = position;
            entity.shader = Resources::instance().shaders.model_shader;
            entity.vertex_array = Resources::instance().models.bush;
            entity.scale = 0.55f;
            entity.rotation = glm::quat(rotation);
            entity.environment = environment;
            entity.id = blue::Context::renderer().add(entity);
            break;
        }
        case (Model::CUT_TREE):
        {
            entity.position = position;
            entity.shader = Resources::instance().shaders.model_shader;
            entity.vertex_array = Resources::instance().models.cut_tree;
            entity.scale = 0.180f;
            entity.rotation = glm::quat(rotation);
            entity.environment = environment;
            entity.id = blue::Context::renderer().add(entity);
            break;
        }
        case (Model::TRACK):
        {
            entity.position = position;
            entity.shader = Resources::instance().shaders.model_shader;
            entity.vertex_array = Resources::instance().models.track;
            entity.scale = 0.180f;
            entity.rotation = glm::quat(rotation);
            entity.environment = environment;
            entity.id = blue::Context::renderer().add(entity);
            break;
        }
        case (Model::BRIDGE):
        {
            entity.position = position;
            entity.shader = Resources::instance().shaders.model_shader;
            entity.vertex_array = Resources::instance().models.bridge;
            entity.scale = 0.35f;
            entity.rotation = glm::quat(rotation);
            entity.environment = environment;
            entity.id = blue::Context::renderer().add(entity);
            break;
        }
        default:
        {
            blue::Context::logger().error("Failed to match model! Value: {}", static_cast<int>(model));
        }
    }
    entries.push_back({model, entity});
    return entity;
}

void Flora::update_entry(const RenderEntity &render_entity)
{
    auto iter = std::find_if(entries.begin(), entries.end(), [render_entity](const Entry &e)
    { return e.entity.id == render_entity.id; });

    if(iter != entries.end())
    {
        iter->entity.rotation = render_entity.rotation;
        iter->entity.position = render_entity.position;
    }
}
