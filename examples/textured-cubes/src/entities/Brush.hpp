//
// Created by dbeef on 5/26/19.
//

#ifndef OPENGL_PLAYGROUND_BRUSH_HPP
#define OPENGL_PLAYGROUND_BRUSH_HPP

#include "blue/voxel/Chunk.h"
#include "blue/RenderStep.h"
#include "blue/RenderStepSequence.h"
#include "blue/BasicRenderStepSequence.hpp"
#include <Resources.h>
#include <atomic>

class Brush {

public:

    enum class Highlight {
        BLUE,
        RED
    };

    Brush() : _size(MIN_SIZE) {}

    void load_models();

    void render();

    inline void show() {
        _hidden = false;
    }

    inline void hide() {
        _hidden = true;
    }

    void set_position(const glm::vec3 pos) {
        RenderStep step(_mesh->toRenderStep());
        step.position = pos;
        _blue_sequence->update_position(step);
        _red_sequence->update_position(step);
    }

    void highlight_blue() {
        _highlight = Highlight::BLUE;
    }

    void highlight_red() {
        _highlight = Highlight::RED;
    }

private:

    static constexpr int MAX_SIZE = 1;
    static constexpr int MIN_SIZE = 1;

    int _size;

    std::shared_ptr<Mesh> _mesh;
    std::shared_ptr<BasicRenderStepSequence> _blue_sequence;
    std::shared_ptr<BasicRenderStepSequence> _red_sequence;
    std::atomic_bool _hidden{false};

    std::atomic<Highlight> _highlight{Highlight::BLUE};
};

#endif //OPENGL_PLAYGROUND_BRUSH_HPP
