//
// Created by dbeef on 5/26/19.
//

#include <Application.h>
#include <Resources.h>
#include "ImGuiEditingSummary.hpp"
#include "blue/Context.hpp"

void ImGuiEditingSummary::batchCalls() {

    ImGui::Begin("Level editor; Summary");
    if (blue::Context::input().cursor_attached.load())
        ImGui::TextColored(ImVec4(1, 0, 0, 1), "Cursor attached, press T to detach.");
    else
        ImGui::TextColored(ImVec4(0, 1, 1, 1), "Cursor detached, press R to attach.");

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    bool update_frustum_result = ImGui::Button("Update frustum vertices");
    bool dump_to_file_result = ImGui::Button("Dump to file");

    bool rendering_lines_result = _rendering_lines;
    bool rendering_grid_result = _rendering_grid;
    bool vsync_result = _vsync;
    bool detailed_mesh_result = _detailed_mesh;
    bool rendering_stencil_buffer_result = _rendering_stencil_buffer;

    ImGui::Checkbox("Rendering lines", &rendering_lines_result);
    ImGui::Checkbox("Rendering grid", &rendering_grid_result);
    ImGui::Checkbox("Vertical synchronization", &vsync_result);
    ImGui::Checkbox("Detailed mesh", &detailed_mesh_result);
    ImGui::Checkbox("Stencil buffer shader", &rendering_stencil_buffer_result);

    ImGui::End();

    if (update_frustum_result) {
        Application::instance().frustum->update_vertices();
        Application::instance().frustum_mesh->upload_vertices(Application::instance().frustum->vertices);
    }

    if (rendering_lines_result != _rendering_lines) {
        _rendering_lines = rendering_lines_result;
        if (_rendering_lines)
            Mesh::renderingLines();
        else
            Mesh::renderingFill();
    }

    if (vsync_result != _vsync) {
//        _vsync = vsync_result;
//        if (!_vsync)
//            glfwSwapInterval(0);
//        else
//            glfwSwapInterval(1);
    }

    if (dump_to_file_result) {
        auto serializer = Application::instance().map->serialize();
        serializer.save_to_file("map.bin");

        serializer.read_width();
        serializer.read_height();

        Application::instance().map->generateFromSerializer(serializer);
    }

    if (rendering_stencil_buffer_result != _rendering_stencil_buffer) {
        _rendering_stencil_buffer = rendering_stencil_buffer_result;
        // FIXME: Rendering stencil buffer should be handled differently
        Application::instance().stencil_buffer.store(rendering_stencil_buffer_result);
    }

    if (rendering_grid_result != _rendering_grid) {
        _rendering_grid = rendering_grid_result;
//        if (_rendering_grid)
//            Application::instance().map_renderer->_ready_chunks->program = Resources::instance().shaders.mvp_texture_light_striped;
//        else
//            Application::instance().map_renderer->_ready_chunks->program = Resources::instance().shaders.mvp_texture_light;
    }

    if (_detailed_mesh != detailed_mesh_result) {

        _detailed_mesh = detailed_mesh_result;

        int CHUNKS_X = Application::instance().map->_width;
        int CHUNKS_Y = Application::instance().map->_height;

//        if (_detailed_mesh) {
//            for (int x = 0; x < CHUNKS_X; x++) {
//                for (int y = 0; y < CHUNKS_Y; y++) {
//
//                    auto &chunk = Application::instance().map->getChunk(x, y);
//                    if (!chunk.mesh_low_detail || !chunk.mesh) continue;
//
//                    Application::instance().map_renderer->_ready_chunks->remove_render_step({chunk.mesh_low_detail});
//                    RenderStep step = {chunk.mesh};
//                    step.position = chunk.getMeshPosition();
//                    Application::instance().map_renderer->_ready_chunks->add_render_step(step);
//                }
//            }
//        } else {
//            for (int x = 0; x < CHUNKS_X; x++) {
//                for (int y = 0; y < CHUNKS_Y; y++) {
//
//                    auto &chunk = Application::instance().map->getChunk(x, y);
//                    if (!chunk.mesh_low_detail || !chunk.mesh) continue;
//
//                    Application::instance().map_renderer->_ready_chunks->remove_render_step({chunk.mesh});
//                    RenderStep step = {chunk.mesh_low_detail};
//                    step.position = chunk.getMeshPosition();
//                    Application::instance().map_renderer->_ready_chunks->add_render_step(step);
//                }
//            }
//        }
    }
}

