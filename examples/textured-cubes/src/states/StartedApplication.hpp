//
// Created by dbeef on 5/26/19.
//

#ifndef OPENGL_PLAYGROUND_STARTEDAPPLICATION_HPP
#define OPENGL_PLAYGROUND_STARTEDAPPLICATION_HPP

#include "ApplicationState.hpp"

#include <memory>
#include <imgui/ImGuiGreeting.hpp>

class StartedApplication : public ApplicationState {

public:

    StartedApplication() : _greeting(std::make_unique<ImGuiGreeting>()) {}

    void draw_imgui() override {
        _greeting->batchCalls();
    }

    void on_entry() override {
        std::cout << "Entering StartedApplication" << std::endl;
    }

private:

    std::unique_ptr<ImGuiGreeting> _greeting;

};

#endif //OPENGL_PLAYGROUND_STARTEDAPPLICATION_HPP
