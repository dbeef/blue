#pragma once

#include <string>
#include <vector>

#include "Resources.hpp"
#include "glm/glm.hpp"
#include "blue/Renderer.h"

// Not thread safe.
class Flora
{
public:

    struct Entry
    {
        Model model{};
        RenderEntity entity;
    };

    RenderEntity add_entry(Model model, const glm::vec3& position, const glm::vec3& rotation);
    RenderEntity add_instanced_rendering_entry(Model model, const Instances& instances, std::uint32_t instances_count);
    void remove_entry(const RenderEntity& render_entity);
    void update_entry(const RenderEntity& render_entity);

    void import_from_file(const std::string& filename);
    void export_to_file(const std::string& filename);

private:

    std::vector<Entry> entries;
};
