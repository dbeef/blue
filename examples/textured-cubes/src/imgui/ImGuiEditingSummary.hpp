//
// Created by dbeef on 5/26/19.
//

#ifndef OPENGL_PLAYGROUND_IMGUIEDITING_HPP
#define OPENGL_PLAYGROUND_IMGUIEDITING_HPP

#include "blue/primitives/ImguiWindow.h"
#include "imgui/imgui.h"

class ImGuiEditingSummary : public ImguiWindow {

public:

    void batchCalls() override;

    bool _rendering_frustum = false;
    bool _rendering_stencil_buffer = true;
    bool _rendering_lines = false;
    bool _rendering_grid = true;
    bool _vsync = true;
    bool _detailed_mesh = true;

};

#endif //OPENGL_PLAYGROUND_IMGUIEDITING_HPP
