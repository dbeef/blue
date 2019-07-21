//
// Created by dbeef on 5/26/19.
//

#include <Application.h>
#include "ImGuiNewFlatLevelParameters.hpp"
#include "blue/Context.hpp"

void ImGuiNewFlatLevelParameters::batchCalls() {
    ImGui::Begin("Level editor; New flat level parameters");

    if (blue::Context::input().cursor_attached.load())
        ImGui::TextColored(ImVec4(1, 0, 0, 1), "Cursor attached, press T to detach.");
    else
        ImGui::TextColored(ImVec4(0, 1, 1, 1), "Cursor detached, press R to attach.");

    ImGui::Text("Perlin parameters");

    ImGui::Text("Level width X in chunks");
    ImGui::InputInt("X", &width);
    width = glm::clamp(width, MIN_MAP_SIZE, MAX_MAP_SIZE);

    ImGui::Text("Level width Z in chunks");
    ImGui::InputInt("Z", &height);
    height = glm::clamp(height, MIN_MAP_SIZE, MAX_MAP_SIZE);

    bool result_back = ImGui::Button("Back");
    bool result_generate = ImGui::Button("Generate");

    ImGui::End();

    if (result_back) {
        Application::instance().state_back();
    } else if (result_generate) {
        Application::instance().new_map(width, height);
        auto next_state = Application::instance().editing;
        Application::instance().to_new_state(next_state);
        Application::instance().clear_state_history();
    }
}

