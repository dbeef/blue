#pragma once

#include <cstdint>
#include <blue/Context.hpp>

struct PlacingRules
{
    std::uint8_t width_in_screen_percent{};
    std::uint8_t height_in_screen_percent{};

    std::uint8_t x_in_screen_percent_from_left{};
    std::uint8_t y_in_screen_percent_from_up{};

    void evaluate(std::uint16_t &x, std::uint16_t &y, std::uint16_t &width, std::uint16_t &height) const
    {
        const auto &screen_width = blue::Context::window().get_width();
        const auto &screen_height = blue::Context::window().get_height();

        x = x_in_screen_percent_from_left * screen_width / 100;
        y = y_in_screen_percent_from_up * screen_height / 100;

        width = width_in_screen_percent * screen_width / 100;
        height = height_in_screen_percent * screen_height / 100;
    }
};
