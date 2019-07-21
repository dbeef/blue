//
// Created by dbeef on 5/26/19.
//

#ifndef OPENGL_PLAYGROUND_IMGUIGREETINGWINDOW_HPP
#define OPENGL_PLAYGROUND_IMGUIGREETINGWINDOW_HPP

#include "blue/primitives/ImguiWindow.h"
#include "imgui/imgui.h"

class ImGuiGreeting : public ImguiWindow {

public:

    void batchCalls() override;

};

#endif //OPENGL_PLAYGROUND_IMGUIGREETINGWINDOW_HPP
