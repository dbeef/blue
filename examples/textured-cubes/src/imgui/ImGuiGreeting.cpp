//
// Created by dbeef on 5/26/19.
//

#include <Application.h>
#include "ImGuiGreeting.hpp"
#include "blue/Context.hpp"

void ImGuiGreeting::batchCalls() {
    ImGui::Begin("Level editor; Greeting");

    if (blue::Context::input().cursor_attached.load())
        ImGui::TextColored(ImVec4(1, 0, 0, 1), "Cursor attached, press T to detach.");
    else
        ImGui::TextColored(ImVec4(0, 1, 1, 1), "Cursor detached, press R to attach.");

    bool result_new_perlin_level = ImGui::Button("Create new perlin level");
    bool result_new_flat_level = ImGui::Button("Create new flat level");
    bool result_load_level = ImGui::Button("Load level");

    ImGui::End();

    if (result_new_perlin_level) {
        auto next_state = Application::instance().new_perlin_level;
        Application::instance().to_new_state(next_state);
    } else if (result_new_flat_level) {
        auto next_state = Application::instance().new_flat_level;
        Application::instance().to_new_state(next_state);
    } else if (result_load_level) {
        Application::instance().new_map("map.bin");
        auto next_state = Application::instance().editing;
        Application::instance().to_new_state(next_state);
        Application::instance().clear_state_history();

    }
}

