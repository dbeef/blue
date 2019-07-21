#include <blue/Context.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <limits>
#include <utility>
#include <iostream>
#include <iomanip>
#include <thread>
#include <chrono>
#include <memory>
#include <vector>
#include "input/InputCallbacks.h"

#include "blue/Logger.hpp"
#include "blue/Setup.h"
#include "blue/input/InputHandler.h"
#include "blue/textures/Texture.h"
#include "blue/primitives/Shapes.h"
#include "blue/Renderer.h"
#include "blue/Camera.h"
#include "blue/primitives/VertexArray.h"
#include "blue/primitives/VertexBuffer.h"
#include "blue/primitives/IndexBuffer.h"
#include "blue/shaders/ShaderUtils.h"
#include "blue/shaders/Shader.h"
#include "blue/shaders/ShaderProgram.h"
#include "blue/primitives/IndexBuffer.h"
#include "blue/textures/TextureAtlas.h"
#include "blue/terrain/Map.h"
#include "blue/voxel/Chunk.h"
#include "blue/textures/TextureArray.h"
#include "blue/primitives/ImguiWindow.h"
#include "imgui/ImGuiGeneratorOptionsWindow.h"
#include "input/IntersectionTestJob.h"
#include "input/Input.hpp"
#include "Resources.h"
#include "Application.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_main.h>

void game_loop() {

	//SDL_GL_SetSwapInterval(0);

	std::vector<ShaderProgram> programs;
	programs.push_back(*Resources::instance().shaders.mvp_blue_transparent);
	programs.push_back(*Resources::instance().shaders.mvp_texture_prebaked);
	programs.push_back(*Resources::instance().shaders.mvp_texture);
	programs.push_back(*Resources::instance().shaders.mvp_texture_light_striped);
	programs.push_back(*Resources::instance().shaders.mvp_texture_light);
	programs.push_back(*Resources::instance().shaders.mvp_blue_transparent);
	programs.push_back(*Resources::instance().shaders.mvp_red_transparent);
	programs.push_back(*Resources::instance().shaders.skybox);
	programs.push_back(*Resources::instance().shaders.tank);
	programs.push_back(*Resources::instance().shaders.tank_outline);
	programs.push_back(*Resources::instance().shaders.simple_depth);
	programs.push_back(*Resources::instance().shaders.wireframe);

	blue::Context::renderer().init_uniform_buffer(programs);
	blue::Context::renderer().setup_shadow_shader(Resources::instance().shaders.mvp_texture_light_striped);

	//    Application::instance().camera->cameraPos =
	//            glm::vec3((float) (CHUNKS_X / 2) * CHUNK_SIZE, 50, (float) (CHUNKS_Y / 2) * CHUNK_SIZE);

	Application::instance().camera->cameraPos = glm::vec3(0, 0, 0);


	//    ImguiSummaryWindow imguiSummaryWindow;
	//    ImGuiGeneratorOptionsWindow imguiGeneratorOptionsWindow;

	Application::instance().tanks->colorbuffer_program = Resources::instance().shaders.tank;
	Application::instance().tanks->depthbuffer_program = Resources::instance().shaders.simple_depth;
	Application::instance().tanks->texture = Resources::instance().textures.cube;

	Application::instance().wireframes->program = Resources::instance().shaders.wireframe;
	Application::instance().wireframes->texture = Resources::instance().textures.cube;

	// TODO: move this to map renderer

	Application::instance().map_renderer->_ready_chunks->colorbuffer_program = Resources::instance().shaders.mvp_texture_light_striped;
	Application::instance().map_renderer->_ready_chunks->depthbuffer_program = Resources::instance().shaders.simple_depth;
	Application::instance().map_renderer->_ready_chunks->texture = Resources::instance().textures.cube;

	Application::instance().map_renderer->_rebuilded_chunks->program = Resources::instance().shaders.mvp_blue_transparent;
	Application::instance().map_renderer->_rebuilded_chunks->texture = Resources::instance().textures.cube;

	auto sun_render_sequence = std::make_shared<BasicRenderStepSequence>();
	sun_render_sequence->program = Resources::instance().shaders.mvp_texture_prebaked;
	sun_render_sequence->texture = Resources::instance().textures.cube;
	sun_render_sequence->add_render_step(Application::instance().sun->toRenderStep());

	Application::instance().physics_renderable_bodies->program = Resources::instance().shaders.mvp_texture;
	Application::instance().physics_renderable_bodies->texture = Resources::instance().textures.cube;
	blue::Context::renderer().setClearColor();

	int display_w, display_h;
	SDL_GetWindowSize(blue::Context::window().getWindow(), &display_w, &display_h);
	blue::Context::window().updateSize();
	Application::instance().camera->reset();
	glViewport(0, 0, display_w, display_h);

	auto& frustum = Application::instance().frustum;
	auto& camera = Application::instance().camera;

	Application::instance().frustum->update(*Application::instance().camera.get());
	Application::instance().frustum->update_vertices();
	Application::instance().frustum_mesh->upload_vertices(Application::instance().frustum->vertices);
	//    Application::instance().rebuilded_chunks->add_render_step({Application::instance().frustum_mesh});

	auto& renderer = blue::Context::renderer();
	auto& application = Application::instance();

	while (!Application::instance().exit) {
		//        frustum->update(*camera);
		//        Application::instance().map->updateInFrustum(*frustum, *camera);
		//        static float alpha_rad = 0;
		//        auto origin = Application::instance().camera->cameraPos;
		//        origin.z += 1000 * glm::sin(alpha_rad);
		//        origin.y += 1000 * glm::cos(alpha_rad);
		//        alpha_rad += 0.0001f;
		//        Application::
		//        instance().sun->_position = origin;
		//        blue::Context::renderer().uniform_values.ambient = std::min(1.0f, origin.y / 1000.0f);
		//        blue::Context::renderer().uniform_values.ambient = std::max(0.0f, origin.y / 1000.0f);
		//        blue::Context::renderer().setClearColor();
		//        glfwPollEvents();

				// render others

		renderer.clear();
		renderer.update_common_uniforms(application.camera);
		renderer.upload_common_uniforms(programs);

		// 1: RENDER TO DEPTH BUFFER

		renderer.depthbuffer_drawing_start(application.shadows_framebuffer);
		renderer.draw_depthbuffer(application.map_renderer->_ready_chunks);
		renderer.draw_depthbuffer(application.tanks);
		renderer.depthbuffer_drawing_end(application.shadows_framebuffer, display_w, display_h);
		renderer.draw_colorbuffer(application.map_renderer->_ready_chunks, application.shadows_framebuffer);
		renderer.draw_colorbuffer(application.tanks, application.shadows_framebuffer);

		renderer.draw(application.map_renderer->_rebuilded_chunks);
		renderer.draw(application.physics_renderable_bodies);
		application.editing->_brush->render();
		renderer.draw(application.wireframes);

		// 3: render ui / skybox
		renderer.draw(Resources::instance().skybox, Resources::instance().shaders.skybox);
		application.draw_gui();

		SDL_GL_SwapWindow(blue::Context::window().getWindow());

		// TODO: Should be handled by opengl-context-job-system
		// TODO: Check if physics is thread-safe (most probably not and pushing physics bodies should happen from this thread),
		//   anyway, dedicated physics thread should be good.

		application.opengl_job_queue->process_jobs();
	}

}

int main(int argc, char* argv[]) {

	// TODO: Blue_Init_Context()

	//Logger::init();

	blue::Context::init();
	blue::Context::window().create();

	Application::instance().create_window();

	Application::init();
	registerCallbacks();

	blue::Context::logger().info("Creating window");

	Resources::init();
	blue::Context::logger().info("Loading models");
	Resources::instance().load_models();
	blue::Context::logger().info("Loading shaders");
	Resources::instance().load_shaders();
	blue::Context::logger().info("Loading skybox");
	Resources::instance().load_skybox();
	blue::Context::logger().info("Loading images");
	Resources::instance().load_images();

	Resources::instance().entities.tank->mesh_step.position = { 50, 40, 50 + 0.5f };
	Resources::instance().entities.tank->mesh_step._scale = 1;
	Resources::instance().entities.boulder->mesh_step._scale = 1;
	Resources::instance().entities.tree->mesh_step._scale = 1;
	Resources::instance().entities.grass->mesh_step._scale = 1;

	Application::instance().editing->_brush->load_models();

	Application::instance().cube = Resources::instance().models.cube;
	// FIXME: This is not copying, cube's position is also changed by this
	Application::instance().sun = Resources::instance().models.cube;
	//    Application::instance().sun->_position = glm::vec3(50, 70, 50);
	blue::Context::logger().info("Initializing framebuffer");
	Application::instance().shadows_framebuffer->init();

	// TODO: Make game loop to be inside OpenGL-wrapper,
	//  also decouple rendering from updating, like in 'Game Design Patterns'.
	blue::Context::logger().info("Entering game loop");
	game_loop();

	blue::Context::logger().info("Disposing resources");
	Resources::dispose();
	Application::instance().close_window();
	Application::dispose();
	blue::Context::dispose();

	return 0;
}
