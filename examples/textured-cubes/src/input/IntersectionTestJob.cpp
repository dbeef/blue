//
// Created by dbeef on 5/26/19.
//

#include "IntersectionTestJob.h"

#include <memory>
#include "Input.hpp"
#include "Resources.h"
#include <chrono>
#include <Resources.h>

#include "Application.h"
#include "blue/Context.hpp"

void IntersectionTestJob::execute() {

    using namespace std::chrono_literals;

    const std::shared_ptr<Camera> &camera = Application::instance().camera;

    int CHUNKS_X = Application::instance().map->_width;
    int CHUNKS_Y = Application::instance().map->_height;

    while (!(shutdown.load())) {

        auto start = std::chrono::system_clock::now();
        bool editing = Application::instance().editing->is_editing();

        if (!editing) {
            std::this_thread::sleep_for(16ms);
            continue;
        }

        Cube *best_result_cube = nullptr;
        double closest_distance = std::numeric_limits<double>::max();

        uint32_t best_chunk_x = 0;
        uint32_t best_chunk_y = 0;
        std::size_t cube_x = 0, cube_y = 0, cube_z = 0;

        glm::vec3 direction;
        if (!blue::Context::input().cursor_attached.load()) {

            glm::mat4 projection_matrix = glm::perspective(glm::radians(camera->fov),
                                                           (GLfloat) blue::Context::window().getWidth() /
                                                           (GLfloat)blue::Context::window().getHeight(),
                                                           0.1f, 1000.0f);
            glm::mat4 view_matrix = glm::lookAt(camera->cameraPos, camera->cameraPos + camera->cameraFront,
                                                camera->CAMERA_UP);

            float x = static_cast<float>((2.0f * Application::instance().last_x) / blue::Context::window().getWidth() - 1.0f);
            float y = static_cast<float>(1.0f - (2.0f * Application::instance().last_y) / blue::Context::window().getHeight());

            float z = 1.0f;
            glm::vec3 ray_nds(x, y, z);
            glm::vec4 ray_clip = glm::vec4(ray_nds.x, ray_nds.y, -1.0, 1.0);
            glm::vec4 ray_eye = inverse(projection_matrix) * ray_clip;
            ray_eye = glm::vec4(ray_eye.x, ray_eye.y, -1.0, 0.0);
            glm::vec3 ray_wor = (inverse(view_matrix) * ray_eye);
            direction = glm::normalize(ray_wor);
        } else {
            direction = glm::normalize(camera->cameraFront);
        }

        for (uint32_t chunk_y = 0; chunk_y < CHUNKS_Y; chunk_y++) {
            for (uint32_t chunk_x = 0; chunk_x < CHUNKS_X; chunk_x++) {

                auto &current_chunk = Application::instance().map->getChunk(chunk_x, chunk_y);

                std::lock_guard<std::mutex> guard(current_chunk.accessingCubesMtx);

                // Now check every cube in chunk
                for (std::size_t y = 0; y < CHUNK_SIZE; y++) {
                    for (std::size_t x = 0; x < CHUNK_SIZE; x++) {
                        for (std::size_t z = 0; z < CHUNK_SIZE; z++) {

                            Cube *cube = current_chunk.cubes[x][y][z];
                            if (cube == nullptr || cube->completelyHidden) continue;
                            glm::vec3 offset = current_chunk.getMeshPosition();

                            float intersec_distance;
                            bool result = cube->TestRayOBBIntersection(camera->cameraPos, direction,
                                                                       intersec_distance, offset);

                            if (result) {
                                if (intersec_distance < closest_distance) {
                                    closest_distance = intersec_distance;
                                    best_result_cube = cube;
                                    cube_x = x;
                                    cube_y = y;
                                    cube_z = z;
                                    best_chunk_x = chunk_x;
                                    best_chunk_y = chunk_y;
                                }
                            }
                        }
                    }
                }
            }
        }

        if (best_result_cube) {

            // not aligning tightly to avoid Z-fighting (which occurs when passing glm::vec3(0.5f)
            // also, looks nicer since shows where block would be placed.
            auto &chunk = Application::instance().map->getChunk(best_chunk_x, best_chunk_y);

            glm::vec3 highlight_pos = best_result_cube->_position + chunk.getMeshPosition();

            if (!Application::instance().destroying_blocks->load()) {

                switch (best_result_cube->intersection_side) {
                    case Orientation::CLOSE: {
                        if (cube_z > 0) cube_z--;
                        break;
                    }
                    case Orientation::FAR: {
                        if (cube_z < CHUNK_SIZE) cube_z++;
                        break;
                    }
                    case Orientation::LEFT: {
                        if (cube_x < CHUNK_SIZE) cube_x++;
                        break;
                    }
                    case Orientation::RIGHT: {
                        if (cube_x > 0) cube_x--;
                        break;
                    }
                    case Orientation::DOWN: {
                        if (cube_y > 0) cube_y--;
                        break;
                    }
                    case Orientation::UP: {
                        if (cube_y < CHUNK_SIZE) cube_y++;
                        break;
                    }
                }

                if (best_result_cube->intersection_side == Orientation::LEFT)
                    highlight_pos += glm::vec3(1.50f, 0.5f, 0.5f);
                else if (best_result_cube->intersection_side == Orientation::RIGHT)
                    highlight_pos += glm::vec3(-0.50f, 0.5f, 0.5f);
                else if (best_result_cube->intersection_side == Orientation::FAR)
                    highlight_pos += glm::vec3(0.5f, 0.5f, 1.5f);
                else if (best_result_cube->intersection_side == Orientation::CLOSE)
                    highlight_pos += glm::vec3(0.5f, 0.5f, -0.5f);
                else if (best_result_cube->intersection_side == Orientation::UP)
                    highlight_pos += glm::vec3(0.50f, 1.5f, 0.5f);
                else if (best_result_cube->intersection_side == Orientation::DOWN)
                    highlight_pos += glm::vec3(0.50f, -0.5f, 0.5f);
            } else {
                highlight_pos += glm::vec3(0.50f, 0.5f, 0.5f);
            }

            if (static_cast<ImGuiEditingToolbar::EditMode>(Application::instance().editing->_toolbar->_current_edit_mode)
                == ImGuiEditingToolbar::EditMode::MODEL && Application::instance().place_block->load()) {

                if (Application::instance().editing->_toolbar->_current_model_type == 1) {
                    RenderStep boulder_step = Resources::instance().entities.boulder->_mesh->toRenderStep();
                    RenderStep wireframe = Resources::instance().entities.boulder->_wireframe->toRenderStep();
					
                    boulder_step.position = highlight_pos;
                    wireframe.position = highlight_pos;
					//wireframe.position += Resources::instance().entities.boulder->wireframe_center;

                    Application::instance().tanks->add_render_step(boulder_step);
                    //Application::instance().wireframes->add_render_step(wireframe);
                } else if(Application::instance().editing->_toolbar->_current_model_type == 0){
                    RenderStep tree_step = Resources::instance().entities.tree->_mesh->toRenderStep();
                    RenderStep wireframe = Resources::instance().entities.tree->_wireframe->toRenderStep();
                    wireframe.position = highlight_pos + glm::vec3(0, 6, 0);
                    tree_step.position = highlight_pos + glm::vec3(0, 6, 0);
					//wireframe.position.y -= Resources::instance().entities.tree->wireframe_dimensions.y / 2;
					//wireframe.position += Resources::instance().entities.tree->wireframe_center;
                    Application::instance().tanks->add_render_step(tree_step);
                    //Application::instance().wireframes->add_render_step(wireframe);
                } else if(Application::instance().editing->_toolbar->_current_model_type == 3){
                    RenderStep tree_step = Resources::instance().entities.pine_tree->_mesh->toRenderStep();
                    RenderStep wireframe = Resources::instance().entities.pine_tree->_wireframe->toRenderStep();
                    wireframe.position = highlight_pos + glm::vec3(0, 1.75f, 0);
					tree_step.position = highlight_pos + glm::vec3(0, 1.75f, 0);
					//wireframe.position.y -= Resources::instance().entities.tree->wireframe_dimensions.y / 2;
					//wireframe.position += Resources::instance().entities.tree->wireframe_center;
                    Application::instance().tanks->add_render_step(tree_step);
                    //Application::instance().wireframes->add_render_step(wireframe);
                }
				else if (Application::instance().editing->_toolbar->_current_model_type == 2) 
                {
                    RenderStep grass_step = Resources::instance().entities.grass->_mesh->toRenderStep();
                    RenderStep wireframe = Resources::instance().entities.grass->_wireframe->toRenderStep();
					grass_step._scale = 0.2f;
                    grass_step.position = highlight_pos + glm::vec3(0, -0.25f, 0);
                    wireframe.position = highlight_pos + glm::vec3(0, 0, 0);
					//wireframe.position += Resources::instance().entities.grass->wireframe_center;
                    Application::instance().tanks->add_render_step(grass_step);
                    //Application::instance().wireframes->add_render_step(wireframe);
                }
				else if (Application::instance().editing->_toolbar->_current_model_type == 4) 
                {
                    RenderStep hurdle_step = Resources::instance().entities.hurdle->_mesh->toRenderStep();
                    RenderStep wireframe = Resources::instance().entities.hurdle->_wireframe->toRenderStep();
                    hurdle_step.position = highlight_pos + glm::vec3(0, 0, 0);
                    wireframe.position = highlight_pos + glm::vec3(0, 0, 0);
					//wireframe.position += Resources::instance().entities.grass->wireframe_center;
                    Application::instance().tanks->add_render_step(hurdle_step);
                    //Application::instance().wireframes->add_render_step(wireframe);
                }
				else if (Application::instance().editing->_toolbar->_current_model_type == 5) 
                {
                    RenderStep wheat_step = Resources::instance().entities.wheat->_mesh->toRenderStep();
                    RenderStep wireframe = Resources::instance().entities.wheat->_wireframe->toRenderStep();
					wheat_step.position = highlight_pos + glm::vec3(0, -0.15f, 0);
                    wireframe.position = highlight_pos + glm::vec3(0, 0, 0);
					//wireframe.position += Resources::instance().entities.grass->wireframe_center;
                    Application::instance().tanks->add_render_step(wheat_step);
                    //Application::instance().wireframes->add_render_step(wireframe);
                }

                Application::instance().place_block->store(false);

            } else if (Application::instance().place_block->load() ||
                       Application::instance().remove_block->load()) {

                std::cout << "CLICKED" << std::endl;

//                cube_x = std::min(cube_x, static_cast<std::size_t>(CHUNK_SIZE - 1));
//                cube_y = std::min(cube_y, static_cast<std::size_t>(CHUNK_SIZE - 1));
//                cube_z = std::min(cube_z, static_cast<std::size_t>(CHUNK_SIZE - 1));

                if (Application::instance().remove_block->load() && chunk.cubes[cube_x][cube_y][cube_z]) {
                    delete chunk.cubes[cube_x][cube_y][cube_z];
                    chunk.cubes[cube_x][cube_y][cube_z] = nullptr;
                } else if (!chunk.cubes[cube_x][cube_y][cube_z]) {
                    chunk.cubes[cube_x][cube_y][cube_z] = new Cube(glm::vec3(cube_x, cube_y, cube_z));
                    auto cube_type = static_cast<CubeType>(Application::instance().editing->_toolbar->_current_cube_type);
                    chunk.cubes[cube_x][cube_y][cube_z]->setQuadsTextures(cube_type);
                }

                Application::instance().map->regenerateChunk(best_chunk_x, best_chunk_y);

                std::cout << "PLACED! " << cube_x << " " << cube_y << " " << cube_z << std::endl;
                std::cout << "x! " << best_chunk_x << " " << best_chunk_y << std::endl;
                std::cout << "aaa! " << chunk._x_in_chunks << " " << chunk._z_in_chunks << std::endl;
                std::cout << "Intersection distance! " << closest_distance << std::endl;
                std::cout << "Intersection side: " << static_cast<int>(best_result_cube->intersection_side)
                          << std::endl;

                Application::instance().place_block->store(false);
                Application::instance().remove_block->store(false);
            } else {
                // only highlight
                Application::instance().editing->_brush->show();
                if (Application::instance().destroying_blocks->load()) {
                    Application::instance().editing->_brush->highlight_red();
                    Application::instance().editing->_brush->set_position(highlight_pos);
                } else {
                    Application::instance().editing->_brush->highlight_blue();
                    Application::instance().editing->_brush->set_position(highlight_pos);
                }
            }

        } else {
            Application::instance().editing->_brush->hide();
            Application::instance().place_block->store(false);
            Application::instance().remove_block->store(false);
        }

        auto stop = std::chrono::system_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

        if (elapsed.count() < 16) {
            auto time_left = std::chrono::microseconds(16 - elapsed.count());
            std::this_thread::sleep_for(time_left);
        }

    }

}

