//
// Created by dbeef on 5/26/19.
//

#ifndef OPENGL_PLAYGROUND_IMGUINEWFLATLEVELPARAMETERS_HPP
#define OPENGL_PLAYGROUND_IMGUINEWFLATLEVELPARAMETERS_HPP


#include "blue/primitives/ImguiWindow.h"
#include "imgui/imgui.h"
#include "blue/voxel/Chunk.h"

class ImGuiNewFlatLevelParameters : public ImguiWindow {

public:

    void batchCalls() override;

private:

    int width{};
    int height{};
};



#endif //OPENGL_PLAYGROUND_IMGUINEWFLATLEVELPARAMETERS_HPP
