#include <utility>
#include <blue/Context.hpp>

//
// Created by dbeef on 4/4/19.
//

#ifndef OPENGL_PLAYGROUND_WORLD_H
#define OPENGL_PLAYGROUND_WORLD_H


#include <atomic>
#include <memory>

#include "blue/Setup.h"
#include "blue/Debugging.h"
#include "blue/voxel/Chunk.h"
#include "blue/Window.hpp"
#include "blue/Camera.h"
#include "blue/input/InputHandler.h"
#include "blue/jobs/JobSystem.hpp"
#include "blue/jobs/SpinlockJobQueue.h"
#include "blue/RenderStepSequence.h"
#include "blue/primitives/Frustrum.hpp"
#include "blue/terrain/Map.h"
#include "blue/Framebuffer.hpp"
#include "blue/terrain/MapRenderer.hpp"

#include "input/InputCallbacks.h"

#include "states/StartedApplication.hpp"
#include "states/NewPerlinLevel.hpp"
#include "states/NewFlatLevel.hpp"
#include "states/ApplicationState.hpp"
#include "states/Editing.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_sdl.h"

#include "input/IntersectionTestJob.h"

#include <stack>

#include <SDL2/SDL.h>
#include "input/Input.hpp"

#undef far
#undef FAR
#undef near
#undef NEAR


static constexpr int MAX_MAP_SIZE = 16;
static constexpr int MIN_MAP_SIZE = 1;

class Application {

public:

    void new_map(int width, int height) {
        map = std::make_shared<Map>(width, height, opengl_job_queue, map_renderer);
        map->generateFromPlain();

        auto intersection = std::make_shared<IntersectionTestJob>();
        blue::Context::job_system().dispatch(intersection);
    }

    void new_map(int width, int height, PerlinParameters params) {
        map = std::make_shared<Map>(width, height, opengl_job_queue, map_renderer);
        map->generateFromPerlinNoise(params);

        auto intersection = std::make_shared<IntersectionTestJob>();
		blue::Context::job_system().dispatch(intersection);
    }

    void new_map(const std::string &filename) {

        MapSerializer serializer;
        serializer.load_file(filename);

        map = std::make_shared<Map>(serializer.read_width(), serializer.read_height(), opengl_job_queue, map_renderer);
        map->generateFromSerializer(serializer);

        auto intersection = std::make_shared<IntersectionTestJob>();
		blue::Context::job_system().dispatch(intersection);
    }

    static void init() {
        _instance = new Application();
    }

    static void dispose() {
        delete _instance;
    }

    static Application &instance() {
        return *_instance;
    }

    std::atomic_int vertices_counter{0};
    bool in_frustum{false};

    std::shared_ptr<Framebuffer> shadows_framebuffer;

    // fixme
    std::stack<std::shared_ptr<ApplicationState>> state_history;
    std::shared_ptr<ApplicationState> current_state;
    std::shared_ptr<StartedApplication> started_application;
    std::shared_ptr<NewPerlinLevel> new_perlin_level;
    std::shared_ptr<NewFlatLevel> new_flat_level;
    std::shared_ptr<Editing> editing;

	std::atomic<float> last_x = 0;
	std::atomic<float> last_y = 0;

    std::shared_ptr<MapRenderer> map_renderer;
    std::shared_ptr<Map> map;

    std::shared_ptr<ShadowsRenderStepSequence> tanks;
    std::shared_ptr<BasicRenderStepSequence> wireframes;

    std::shared_ptr<Mesh> frustum_mesh = std::make_shared<Mesh>();

    std::shared_ptr<Frustrum> frustum = std::make_shared<Frustrum>();

    std::atomic_bool exit{false};

    std::shared_ptr<BasicRenderStepSequence> physics_renderable_bodies;
//    std::shared_ptr<q3Scene> physics_scene;
    std::shared_ptr<PerlinParameters> perlin_parameters;
    std::shared_ptr<Camera> camera;

    std::atomic_bool construct_static_blocks{true};
    std::atomic_bool stencil_buffer{true};
    std::shared_ptr<Mesh> cube;
    std::shared_ptr<Mesh> sun;
    glm::vec3 _sunPos;
//    std::vector<q3Body *> physics_bodies;
    std::shared_ptr<SpinlockJobQueue> opengl_job_queue;

    Application() {

        place_block = std::make_shared<std::atomic_bool>(false);
        destroying_blocks = std::make_shared<std::atomic_bool>(false);
        remove_block = std::make_shared<std::atomic_bool>(false);
        rendering_lines = std::make_shared<std::atomic_bool>(false);
        perlin_parameters = std::make_shared<PerlinParameters>();
        camera = std::make_shared<Camera>();
        cube = std::make_shared<Mesh>();
        sun = std::make_shared<Mesh>();
//        physics_scene = std::make_shared<q3Scene>(1.0 / 60.0);
        opengl_job_queue = std::make_shared<SpinlockJobQueue>();
        shadows_framebuffer = std::make_shared<Framebuffer>();

        tanks = std::make_shared<ShadowsRenderStepSequence>();
        wireframes = std::make_shared<BasicRenderStepSequence>();

//        physics_scene->SetAllowSleep( false );
//        physics_scene->SetEnableFriction( true );
//        physics_scene->SetIterations( 100 );
        physics_renderable_bodies = std::make_shared<BasicRenderStepSequence>();

        started_application = std::make_shared<StartedApplication>();
        new_perlin_level = std::make_shared<NewPerlinLevel>();
        new_flat_level = std::make_shared<NewFlatLevel>();
        editing = std::make_shared<Editing>();

        map_renderer = std::make_shared<MapRenderer>();

        to_new_state(started_application);
    }

    void to_new_state(std::shared_ptr<ApplicationState> state) {
        current_state = state;
        current_state->on_entry();
        state_history.push(current_state);
    }

    void state_back() {
        if (state_history.size() > 1) {
            state_history.pop();
            current_state = state_history.top();
            current_state->on_entry();
        }
    }

    void clear_state_history() {
        while (state_history.size() > 1) state_history.pop();
    }

    void draw_gui() {

		blue::Context::input().poll();

        ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame(blue::Context::window().getWindow());
        ImGui::NewFrame();
        Application::instance().current_state->draw_imgui();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void create_window() {
        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        (void) io;
        ImGui::StyleColorsDark();
        ImGui_ImplSDL2_InitForOpenGL(blue::Context::window().getWindow(), blue::Context::window().getContext());
#ifdef ANDROID
        ImGui_ImplOpenGL3_Init("#version 300 es");
#else
        ImGui_ImplOpenGL3_Init("#version 330 core");
#endif

        // Enabling blending for transparency.
        DebugGlCall(glEnable(GL_BLEND));
        DebugGlCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    }

    void close_window() {
//        glfwTerminate();
    }

    std::shared_ptr<std::atomic_bool> destroying_blocks;
    std::shared_ptr<std::atomic_bool> place_block;
    std::shared_ptr<std::atomic_bool> remove_block;
    std::shared_ptr<std::atomic_bool> rendering_lines;

private:

    static Application *_instance;

};

#endif //OPENGL_PLAYGROUND_WORLD_H
