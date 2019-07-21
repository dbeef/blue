#include <utility>

//
// Created by dbeef on 4/4/19.
//

#ifndef OPENGL_PLAYGROUND_IMGUIOPTIONSWINDOW_H
#define OPENGL_PLAYGROUND_IMGUIOPTIONSWINDOW_H

#include "imgui/imgui.h"
#include "blue/primitives/ImguiWindow.h"

class ImGuiGeneratorOptionsWindow : public ImguiWindow {

public:

    void batchCalls() override;
};

#endif //OPENGL_PLAYGROUND_IMGUIOPTIONSWINDOW_H
