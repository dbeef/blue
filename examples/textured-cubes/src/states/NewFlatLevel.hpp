//
// Created by dbeef on 5/26/19.
//

#ifndef OPENGL_PLAYGROUND_NEWFLATLEVEL_HPP
#define OPENGL_PLAYGROUND_NEWFLATLEVEL_HPP


#include "ApplicationState.hpp"

#include <memory>
#include <iostream>
#include <imgui/ImGuiNewFlatLevelParameters.hpp>

class NewFlatLevel : public ApplicationState {

public:

    NewFlatLevel() : _new_level_parameters(std::make_unique<ImGuiNewFlatLevelParameters>()) {}

    void draw_imgui() override {
        _new_level_parameters->batchCalls();
    }

    void on_entry() override {
        std::cout << "Entering NewFlatLevel" << std::endl;
    }

private:

    std::unique_ptr<ImGuiNewFlatLevelParameters> _new_level_parameters;

};


#endif //OPENGL_PLAYGROUND_NEWFLATLEVEL_HPP
