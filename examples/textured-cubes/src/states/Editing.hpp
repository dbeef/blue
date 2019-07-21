//
// Created by dbeef on 5/26/19.
//

#ifndef OPENGL_PLAYGROUND_EDITING_HPP
#define OPENGL_PLAYGROUND_EDITING_HPP

#include "ApplicationState.hpp"

#include <memory>
#include <iostream>
#include <imgui/ImGuiNewFlatLevelParameters.hpp>
#include <imgui/ImGuiEditingSummary.hpp>
#include <imgui/ImGuiEditingToolbar.hpp>
#include <entities/Brush.hpp>

class Editing : public ApplicationState {

public:

    Editing() : _summary(std::make_unique<ImGuiEditingSummary>()),
                _toolbar(std::make_unique<ImGuiEditingToolbar>()),
                _brush(std::make_shared<Brush>()) {}

    void draw_imgui() override {
        _summary->batchCalls();
        _toolbar->batchCalls();
    }

    void on_entry() override {
        std::cout << "Entering Editing" << std::endl;
        editing.store(true);
    }

    inline bool is_editing() {
        return editing.load();
    }

    std::shared_ptr<Brush> _brush;
    std::unique_ptr<ImGuiEditingSummary> _summary;
    std::unique_ptr<ImGuiEditingToolbar> _toolbar;

private:

    std::atomic_bool editing{false};

};

#endif //OPENGL_PLAYGROUND_EDITING_HPP
