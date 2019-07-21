//
// Created by dbeef on 5/26/19.
//

#ifndef OPENGL_PLAYGROUND_IMGUINEWLEVELPARAMETERS_HPP
#define OPENGL_PLAYGROUND_IMGUINEWLEVELPARAMETERS_HPP


#include "blue/primitives/ImguiWindow.h"
#include "blue/voxel/Chunk.h"
#include "imgui/imgui.h"

class ImGuiNewPerlinLevelParameters : public ImguiWindow {

public:

    void batchCalls() override;

private:

    PerlinParameters parameters{};

    int width{};
    int height{};
};


#endif //OPENGL_PLAYGROUND_IMGUINEWLEVELPARAMETERS_HPP
