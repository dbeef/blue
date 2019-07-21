//
// Created by dbeef on 5/4/19.
//

#include "Resources.h"
#include "Application.h"
#include <iostream>
#include "glad/glad.h"
#include "blue/shaders/ShaderUtils.h"
#include "blue/shaders/Shader.h"
#include <cassert>
#include "blue/primitives/Cube.h"
#include "blue/ModelLoader.h"

Resources *Resources::_instance = nullptr;

static ShaderProgram compile_shader(const std::string &vertex_path, const std::string &fragment_path) {

    std::string vertex_source = ShaderUtils::parseFile(vertex_path);
    std::string fragment_source = ShaderUtils::parseFile(fragment_path);

    Shader vertex_shader(vertex_path, vertex_source, GL_VERTEX_SHADER);
    Shader fragment_shader(fragment_path, fragment_source, GL_FRAGMENT_SHADER);

    ShaderProgram program{};
    program.init(vertex_shader, fragment_shader);
    return program;
}

void Resources::load_images() {
    {
        // cube textures
        std::vector<std::string> filenames;
        filenames.emplace_back("resources/images/grass/top.jpg");
        filenames.emplace_back("resources/images/grass/side.jpg");
        filenames.emplace_back("resources/images/dirt/dirt.jpg");
        filenames.emplace_back("resources/images/stone/stone.jpg");
        filenames.emplace_back("resources/images/navigation/up.jpg");
        filenames.emplace_back("resources/images/navigation/bottom.jpg");
        filenames.emplace_back("resources/images/navigation/left.jpg");
        filenames.emplace_back("resources/images/navigation/right.jpg");
        filenames.emplace_back("resources/images/navigation/close.jpg");
        filenames.emplace_back("resources/images/navigation/far.jpg");
        filenames.emplace_back("resources/images/sun/sun.jpg");
        textures.cube = std::make_shared<TextureArray>();
		textures.cube->init(filenames);
    }
}

void Resources::load_shaders() {

    {
        std::cout << "Loading textured MVP shader with light, striped" << std::endl;
        const std::string vertex_path = "resources/shaders/MVPTextureLightStriped.vertex.glsl";
        const std::string fragment_path = "resources/shaders/MVPTextureLightStriped.fragment.glsl";
        auto program = compile_shader(vertex_path, fragment_path);
        shaders.mvp_texture_light_striped = std::make_shared<ShaderProgram>(program);
    }

    {
        std::cout << "Loading textured MVP shader with light" << std::endl;
        const std::string vertex_path = "resources/shaders/MVPTextureLight.vertex.glsl";
        const std::string fragment_path = "resources/shaders/MVPTextureLight.fragment.glsl";
        auto program = compile_shader(vertex_path, fragment_path);
        shaders.mvp_texture_light = std::make_shared<ShaderProgram>(program);
    }

    {
        std::cout << "Loading textured prebaked MVP shader." << std::endl;
        const std::string vertex_path = "resources/shaders/TexturedBillboard.vertex.glsl";
        const std::string fragment_path = "resources/shaders/MVPTexture.fragment.glsl";
        auto program = compile_shader(vertex_path, fragment_path);
        shaders.mvp_texture_prebaked = std::make_shared<ShaderProgram>(program);
    }

    {
        std::cout << "Loading textured MVP shader." << std::endl;
        const std::string vertex_path = "resources/shaders/MVPTexture.vertex.glsl";
        const std::string fragment_path = "resources/shaders/MVPTexture.fragment.glsl";
        auto program = compile_shader(vertex_path, fragment_path);
        shaders.mvp_texture = std::make_shared<ShaderProgram>(program);
    }

    {
        std::cout << "Loading color, half-transparent MVP shader." << std::endl;
        const std::string vertex_path = "resources/shaders/MVPTexture.vertex.glsl";
        const std::string fragment_path = "resources/shaders/MVPTextureTransparent.fragment.glsl";
        auto program = compile_shader(vertex_path, fragment_path);
        shaders.mvp_blue_transparent = std::make_shared<ShaderProgram>(program);
    }

    {
        std::cout << "Loading color, half-transparent MVP shader." << std::endl;
        const std::string vertex_path = "resources/shaders/MVPTexture.vertex.glsl";
        const std::string fragment_path = "resources/shaders/MVPTextureTransparentRed.fragment.glsl";
        auto program = compile_shader(vertex_path, fragment_path);
        shaders.mvp_red_transparent = std::make_shared<ShaderProgram>(program);
    }

    {
        std::cout << "Loading simple depth shader." << std::endl;
        const std::string vertex_path = "resources/shaders/SimpleDepth.vertex.glsl";
        const std::string fragment_path = "resources/shaders/SimpleDepth.fragment.glsl";
        auto program = compile_shader(vertex_path, fragment_path);
        shaders.simple_depth = std::make_shared<ShaderProgram>(program);
    }

    {
        std::cout << "Loading wireframe shader." << std::endl;
        const std::string vertex_path = "resources/shaders/Wireframe.vertex.glsl";
        const std::string fragment_path = "resources/shaders/Wireframe.fragment.glsl";
        auto program = compile_shader(vertex_path, fragment_path);
        shaders.wireframe = std::make_shared<ShaderProgram>(program);
    }

    {
        std::cout << "Loading skybox shader." << std::endl;
        const std::string vertex_path = "resources/shaders/Skybox.vertex.glsl";
        const std::string fragment_path = "resources/shaders/Skybox.fragment.glsl";
        auto program = compile_shader(vertex_path, fragment_path);
        shaders.skybox = std::make_shared<ShaderProgram>(program);
    }

    {
        std::cout << "Loading tank shader." << std::endl;
        const std::string vertex_path = "resources/shaders/Tank.vertex.glsl";
        const std::string fragment_path = "resources/shaders/Tank.fragment.glsl";
        auto program = compile_shader(vertex_path, fragment_path);
        shaders.tank = std::make_shared<ShaderProgram>(program);
    }

    {
        std::cout << "Loading tank outline shader." << std::endl;
        const std::string vertex_path = "resources/shaders/Tank.vertex.glsl";
        const std::string fragment_path = "resources/shaders/TankOutline.fragment.glsl";
        auto program = compile_shader(vertex_path, fragment_path);
        shaders.tank_outline = std::make_shared<ShaderProgram>(program);
    }

    std::cout << "Done initializing shaders." << std::endl;
}

Resources &Resources::instance() {
    return *_instance;
}

void Resources::init() {
    _instance = new Resources();
}

void Resources::dispose() {
    delete _instance;
}

void Resources::load_models() {
    {
        std::cout << "Loading cube mesh" << std::endl;
        std::shared_ptr<Cube> constructionCube = std::make_shared<Cube>(glm::vec3(0));
        constructionCube->setQuadsTextures(CubeType::SUN);
        constructionCube->updateNeighbouredSides();
        constructionCube->_position = glm::vec3(0);
        constructionCube->vertices();
        models.cube = std::make_shared<Mesh>();
        models.cube->upload_vertices(constructionCube->vertices());
    }
    {
        std::cout << "Loading tank" << std::endl;
        float scale;
        const aiScene* scene_ptr = models::load_scene("resources/models/my_tank.fbx", scale);
        if(scene_ptr) {
            std::cout << "Tank loaded successfuly." << std::endl;
//            models.tank = scene_ptr;
            entities.tank = std::make_shared<Tank>();
            entities.tank->load(scene_ptr);
//            entities.tank->step._scale = scale;

            aiVector3D min;
            aiVector3D max;
            models::get_bounding_box(scene_ptr, &min, &max);
            glm::vec3 glm_min(min.x, min.y, min.z);
            glm::vec3 glm_max(max.x, max.y, max.z);
            entities.tank->load_wireframe(glm_min, glm_max);

        }
        else {
            std::cout << "Tank loading failed." << std::endl;
        }
    }
    {
        std::cout << "Loading tree" << std::endl;
        float scale;
        const aiScene* scene_ptr = models::load_scene("resources/models/tree.fbx", scale);
        if(scene_ptr) {
            std::cout << "tree loaded successfuly." << std::endl;
//            models.tree = scene_ptr;
            entities.tree = std::make_shared<Tank>();
            entities.tree->load(scene_ptr);
//            entities.tree->step._scale = scale;

            aiVector3D min;
            aiVector3D max;
            models::get_bounding_box(scene_ptr, &min, &max);
            glm::vec3 glm_min(min.x, min.y, min.z);
            glm::vec3 glm_max(max.x, max.y, max.z);
            entities.tree->load_wireframe(glm_min, glm_max);

        }
        else {
            std::cout << "tree loading failed." << std::endl;
        }
    }
    {
        std::cout << "Loading boulder" << std::endl;
        float scale;
        const aiScene* scene_ptr = models::load_scene("resources/models/boulder.fbx", scale);
        if(scene_ptr) {
            std::cout << "boulder loaded successfuly." << std::endl;
//            models.boulder = scene_ptr;
            entities.boulder = std::make_shared<Tank>();
            entities.boulder->load(scene_ptr);
//            entities.boulder->step._scale = scale;

            aiVector3D min;
            aiVector3D max;
            models::get_bounding_box(scene_ptr, &min, &max);
            glm::vec3 glm_min(min.x, min.y, min.z);
            glm::vec3 glm_max(max.x, max.y, max.z);
            entities.boulder->load_wireframe(glm_min, glm_max);

        }
        else {
            std::cout << "boulder loading failed." << std::endl;
        }
    } 
	{
        std::cout << "Loading pine tree" << std::endl;
        float scale;
        const aiScene* scene_ptr = models::load_scene("resources/models/pine_tree.fbx", scale);
        if(scene_ptr) {
            std::cout << "pine tree loaded successfuly." << std::endl;
//            models.boulder = scene_ptr;
            entities.pine_tree = std::make_shared<Tank>();
            entities.pine_tree->load(scene_ptr);
//            entities.boulder->step._scale = scale;

            aiVector3D min;
            aiVector3D max;
            models::get_bounding_box(scene_ptr, &min, &max);
            glm::vec3 glm_min(min.x, min.y, min.z);
            glm::vec3 glm_max(max.x, max.y, max.z);
            entities.pine_tree->load_wireframe(glm_min, glm_max);

        }
        else {
            std::cout << "pine_tree loading failed." << std::endl;
        }
    }
    {
        std::cout << "Loading grass" << std::endl;
        float scale;
        const aiScene* scene_ptr = models::load_scene("resources/models/grass.fbx", scale);
        if(scene_ptr) {
            std::cout << "grass loaded successfuly." << std::endl;
//            models.boulder = scene_ptr;
            entities.grass = std::make_shared<Tank>();
            entities.grass->load(scene_ptr);
//            entities.boulder->step._scale = scale;

			
            aiVector3D min;
            aiVector3D max;
            models::get_bounding_box(scene_ptr, &min, &max);
            glm::vec3 glm_min(min.x, min.y, min.z);
            glm::vec3 glm_max(max.x, max.y, max.z);
            entities.grass->load_wireframe(glm_min, glm_max);

        }
        else {
            std::cout << "grass loading failed." << std::endl;
        }
    }
    {
        std::cout << "Loading hurdle" << std::endl;
        float scale;
        const aiScene* scene_ptr = models::load_scene("resources/models/hurdle.fbx", scale);
        if(scene_ptr) {
            std::cout << "hurdle loaded successfuly." << std::endl;
//            models.boulder = scene_ptr;
            entities.hurdle = std::make_shared<Tank>();
            entities.hurdle->load(scene_ptr);
//            entities.boulder->step._scale = scale;

            aiVector3D min;
            aiVector3D max;
            models::get_bounding_box(scene_ptr, &min, &max);
            glm::vec3 glm_min(min.x, min.y, min.z);
            glm::vec3 glm_max(max.x, max.y, max.z);
            entities.hurdle->load_wireframe(glm_min, glm_max);

        }
        else {
            std::cout << "hurdle loading failed." << std::endl;
        }
    }
    {
        std::cout << "Loading wheat" << std::endl;
        float scale;
        const aiScene* scene_ptr = models::load_scene("resources/models/wheat.fbx", scale);
        if(scene_ptr) {
            std::cout << "wheat loaded successfuly." << std::endl;
//            models.boulder = scene_ptr;
            entities.wheat = std::make_shared<Tank>();
            entities.wheat->load(scene_ptr);
//            entities.boulder->step._scale = scale;

            aiVector3D min;
            aiVector3D max;
            models::get_bounding_box(scene_ptr, &min, &max);
            glm::vec3 glm_min(min.x, min.y, min.z);
            glm::vec3 glm_max(max.x, max.y, max.z);
            entities.wheat->load_wireframe(glm_min, glm_max);

        }
        else {
            std::cout << "wheat loading failed." << std::endl;
        }
    }
}

void Resources::load_skybox() {

    std::cout << "Loading skybox" << std::endl;

    skybox = std::make_shared<Skybox>();

    const std::string right = "resources/images/sky_sides.png";
    const std::string left = "resources/images/sky_sides.png";
    const std::string top = "resources/images/sky_bot.png";
    const std::string bottom = "resources/images/sky_top.png";
    const std::string back = "resources/images/sky_sides.png";
    const std::string front = "resources/images/sky_sides.png";

    skybox->load(right, left, top, bottom, back, front);

    std::cout << "Loading skybox done" << std::endl;
}

void Resources::load_entities() {
}

