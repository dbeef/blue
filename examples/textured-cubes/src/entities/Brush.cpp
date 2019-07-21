//
// Created by dbeef on 5/26/19.
//

#include "Brush.hpp"
#include "blue/Context.hpp"

void Brush::load_models() {

    // TODO: Load models for all brush sizes, for now brush is only 1x1
    std::shared_ptr<Cube> constructionCube = std::make_shared<Cube>(glm::vec3(0));
    constructionCube->updateNeighbouredSides();
    constructionCube->_position = glm::vec3(0, 0, 0);

    _mesh = std::make_shared<Mesh>();
    _mesh->upload_indices_vertices(constructionCube->vertices(), constructionCube->indices());

    _blue_sequence = std::make_shared<BasicRenderStepSequence>();
    _blue_sequence->program = Resources::instance().shaders.mvp_blue_transparent;
    _blue_sequence->texture = Resources::instance().textures.cube;

    _red_sequence = std::make_shared<BasicRenderStepSequence>();
    _red_sequence->program = Resources::instance().shaders.mvp_red_transparent;
    _red_sequence->texture = Resources::instance().textures.cube;

    RenderStep red_step = _mesh->toRenderStep();
    red_step._scale = 1.2f;

    _blue_sequence->add_render_step(_mesh->toRenderStep());
    _red_sequence->add_render_step(red_step);
}

void Brush::render() {

    if(_hidden) return;

    switch (_highlight) {
        case Highlight::BLUE:
            blue::Context::renderer().draw(_blue_sequence);
            break;
        case Highlight::RED:
            blue::Context::renderer().draw(_red_sequence);
            break;
    }
}

