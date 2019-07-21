//
// Created by dbeef on 4/28/19.
//

#ifndef OPENGL_PLAYGROUND_RESOURCESMANAGER_H
#define OPENGL_PLAYGROUND_RESOURCESMANAGER_H

#include <memory>
#include "blue/shaders/ShaderProgram.h"
#include "blue/textures/TextureArray.h"
#include "blue/primitives/Mesh.h"
#include "blue/textures/Skybox.hpp"
#include "entities/Tank.h"

class Resources {

public:

    static Resources& instance();

    static void init();

    static void dispose();

    void load_shaders();

    void load_images();

    void load_models();

    void load_skybox();

    void load_entities();

    struct {
        std::shared_ptr<ShaderProgram> mvp_texture_prebaked{};
        std::shared_ptr<ShaderProgram> mvp_texture{};
        std::shared_ptr<ShaderProgram> mvp_texture_light_striped{};
        std::shared_ptr<ShaderProgram> mvp_texture_light{};
        std::shared_ptr<ShaderProgram> mvp_blue_transparent{};
        std::shared_ptr<ShaderProgram> mvp_red_transparent{};
        std::shared_ptr<ShaderProgram> skybox{};
        std::shared_ptr<ShaderProgram> tank{};
        std::shared_ptr<ShaderProgram> tank_outline{};
        std::shared_ptr<ShaderProgram> simple_depth{};
        std::shared_ptr<ShaderProgram> wireframe{};
    } shaders;

    struct {
        std::shared_ptr<TextureArray> cube{};
    } textures;

    struct {
        std::shared_ptr<Mesh> cube;
        aiScene* tank;
        aiScene* tree;
        aiScene* boulder;
    } models;

    struct {
        // fixme obviously
        std::shared_ptr<Tank> tank;
        std::shared_ptr<Tank> tree;
        std::shared_ptr<Tank> boulder;
        std::shared_ptr<Tank> grass;
		std::shared_ptr<Tank> pine_tree;
		std::shared_ptr<Tank> hurdle;
		std::shared_ptr<Tank> wheat;
    } entities;

    std::shared_ptr<Skybox> skybox{};

private:

    static Resources* _instance;

};

#endif //OPENGL_PLAYGROUND_RESOURCESMANAGER_H
