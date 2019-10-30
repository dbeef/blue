// TODO: 'gui' should be a subproject for blue.

#pragma once

#include <blue/Renderer.h>
#include <blue/gui/PlacingRules.h>

class Button
{

public:

    static void init(); // Not thread safe.

    void create(const UniformBufferId& environment, const Texture& clicked, const Texture& idle);
    void update_placing_rules(const PlacingRules&);

    void set_clicked(bool clicked);
    bool is_clicked() { return _clicked;}
    bool collision(std::uint16_t click_x, std::uint16_t click_y) const;

    void dispose();

private:

    std::uint16_t _x_center{};
    std::uint16_t _y_center{};
    std::uint16_t _width{};
    std::uint16_t _height{};

    bool _clicked{};
    PlacingRules _rules{};
    RenderEntity _entity{};
};
