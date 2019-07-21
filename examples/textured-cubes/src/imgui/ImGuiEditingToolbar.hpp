//
// Created by dbeef on 5/26/19.
//

#ifndef OPENGL_PLAYGROUND_IMGUIEDITINGTOOLBAR_HPP
#define OPENGL_PLAYGROUND_IMGUIEDITINGTOOLBAR_HPP


#include "blue/primitives/ImguiWindow.h"
#include "imgui/imgui.h"

class ImGuiEditingToolbar : public ImguiWindow {

public:

    enum class EditMode {
        TERRAIN,
        MODEL
    };

    void batchCalls() override;

    int _current_edit_mode{};
    int _current_cube_type{};
    int _current_model_type{};

};

#endif //OPENGL_PLAYGROUND_IMGUIEDITINGTOOLBAR_HPP
