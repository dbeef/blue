#include "blue/GpuThread.hpp"
#include "blue/Context.hpp"
#include "blue/Timestep.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_opengl3.h"

namespace
{
	void init_imgui()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		(void)io;
		ImGui::StyleColorsDark();
		ImGui_ImplSDL2_InitForOpenGL(blue::Context::window().get_window(), blue::Context::window().get_context());
#ifdef ANDROID
		ImGui_ImplOpenGL3_Init("#version 300 es");
#else
		ImGui_ImplOpenGL3_Init("#version 330 core");
#endif
	}

	inline void imgui_start()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame(blue::Context::window().get_window());
		ImGui::NewFrame();
	}

	inline void imgui_end()
	{
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
}

bool GpuThread::is_running()
{
	return _thread_running.load();
}

bool GpuThread::run()
{
	_thread_running.store(true);

	// Wait for OpenGL context initialisation result.
	auto start_thread_future = _start_thread_status.get_future();
	_thread = std::thread(&GpuThread::render_thread_loop, this);
	start_thread_future.wait();

	if (start_thread_future.get())
	{
		blue::Context::logger().info("Started GPU thread.");
		return true;
	}
	else
	{
		blue::Context::logger().info("Failed to start GPU thread.");
		return false;
	}
}

void GpuThread::stop()
{
	_thread_running.store(false);
	_thread.join();
	blue::Context::logger().info("Stopped GPU thread.");
}

void GpuThread::render_thread_loop()
{
	const auto& window = blue::Context::window();
	auto& gpu_system = blue::Context::gpu_system();
	auto& renderer = blue::Context::renderer();

	bool context_init_success = blue::Context::window().init_gl_context();
	_start_thread_status.set_value(context_init_success);
	if (!context_init_success)
	{
		return;
	}

	Timestep timestep(60, true);

#if defined(BLUE_WINDOWS) || defined(BLUE_LINUX)
	init_imgui();
#endif

	while (blue::Context::gpu_thread().is_running())
	{
		timestep.mark_start();

		renderer.lock();

		renderer.clear();
		renderer.draw_render_entities();

#if defined(BLUE_WINDOWS) || defined(BLUE_LINUX)
		imgui_start();
		renderer.draw_imgui_entities();
		imgui_end();
#endif

		renderer.unlock();

		window.swap_buffers();

		gpu_system.lock();
		bool executed_some_work = gpu_system.execute();
		if (executed_some_work)
        {
		    renderer.invalidate_cache();
        }
		gpu_system.unlock();

		timestep.mark_end();
		timestep.delay();
	}
}
