//
// Created by dbeef on 5/26/19.
//

#include <Application.h>
#include "ImGuiNewPerlinLevelParameters.hpp"
#include "blue/Context.hpp"

void ImGuiNewPerlinLevelParameters::batchCalls() {
    ImGui::Begin("Level editor; New perlin level parameters");

    if (blue::Context::input().cursor_attached.load())
        ImGui::TextColored(ImVec4(1, 0, 0, 1), "Cursor attached, press T to detach.");
    else
        ImGui::TextColored(ImVec4(0, 1, 1, 1), "Cursor detached, press R to attach.");


    ImGui::Text("Perlin parameters");

    ImGui::Text("Level width X in chunks");
    ImGui::InputInt("", &width);
    width = glm::clamp(width, MIN_MAP_SIZE, MAX_MAP_SIZE);

    ImGui::Text("Level width Z in chunks");
    ImGui::InputInt("", &height);
    height = glm::clamp(height, MIN_MAP_SIZE, MAX_MAP_SIZE);

    ImGui::SliderFloat("Frequency", &parameters.frequency, 0.001f, 15.0f);
    ImGui::SliderFloat("Amplitude", &parameters.amplitude, 0.001f, 15.0f);
    ImGui::SliderFloat("Lacunarity", &parameters.lacunarity, 0.001f, 15.0f);
    ImGui::SliderFloat("Persistence", &parameters.persistence, 0.001f, 15.0f);

    bool result_back = ImGui::Button("Back");
    bool result_generate = ImGui::Button("Generate");

    ImGui::End();

    if (result_back) {
        Application::instance().state_back();
    } else if (result_generate) {
        Application::instance().new_map(width, height, parameters);
        auto next_state = Application::instance().editing;
        Application::instance().to_new_state(next_state);
        Application::instance().clear_state_history();
    }
}

