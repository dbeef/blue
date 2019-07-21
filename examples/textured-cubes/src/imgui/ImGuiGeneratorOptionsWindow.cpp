//
// Created by dbeef on 5/26/19.
//

#include "ImGuiGeneratorOptionsWindow.h"

#include "Application.h"
#include "blue/Context.hpp"

void ImGuiGeneratorOptionsWindow::batchCalls() {

    ImGui::Begin("Generator options");

    if (blue::Context::input().cursor_attached.load())
        ImGui::TextColored(ImVec4(1, 0, 0, 1), "Cursor attached, press T to detach.");
    else
        ImGui::TextColored(ImVec4(0, 1, 1, 1), "Cursor detached, press R to attach.");

    ImGui::Text("Perlin parameters");
    ImGui::SliderFloat("Frequency", &Application::instance().perlin_parameters->frequency, 0.001f, 15.0f);
    ImGui::SliderFloat("Amplitude", &Application::instance().perlin_parameters->amplitude, 0.001f, 15.0f);
    ImGui::SliderFloat("Lacunarity", &Application::instance().perlin_parameters->lacunarity, 0.001f, 15.0f);
    ImGui::SliderFloat("Persistence", &Application::instance().perlin_parameters->persistence, 0.001f, 15.0f);
    ImGui::Text("Vertices counter: %lu", Application::instance().vertices_counter.load() / 9);
    ImGui::Text("Light parameters");
    float value_before = blue::Context::renderer().uniform_values.ambient;
    ImGui::SliderFloat("Ambient", &blue::Context::renderer().uniform_values.ambient, 0.0f, 1.0f);
    bool result = ImGui::Button("Generate");

    if (result && !Application::instance().map->_generating) {
        std::cout << "Starting new thread!" << std::endl;
        const auto &parameters = *Application::instance().perlin_parameters;
        Application::instance().map->generateFromPerlinNoise(parameters);
    }

    if (value_before != blue::Context::renderer().uniform_values.ambient) {
        blue::Context::renderer().setClearColor();
    }

    ImGui::End();
}

