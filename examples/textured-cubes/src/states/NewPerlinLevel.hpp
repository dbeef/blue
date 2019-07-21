//
// Created by dbeef on 5/26/19.
//

#ifndef OPENGL_PLAYGROUND_NEWLEVEL_HPP
#define OPENGL_PLAYGROUND_NEWLEVEL_HPP

#include "ApplicationState.hpp"

#include <memory>
#include <iostream>
#include <imgui/ImGuiNewPerlinLevelParameters.hpp>

class NewPerlinLevel : public ApplicationState {

public:

    NewPerlinLevel() : _new_level_parameters(std::make_unique<ImGuiNewPerlinLevelParameters>()) {}

    void draw_imgui() override {
        _new_level_parameters->batchCalls();
    }

    void on_entry() override {
        std::cout << "Entering NewPerlinLevel" << std::endl;
    }

private:

    std::unique_ptr<ImGuiNewPerlinLevelParameters> _new_level_parameters;

};


#endif //OPENGL_PLAYGROUND_NEWLEVEL_HPP
