#include "blue/GpuThread.hpp"
#include "blue/Context.hpp"
#include "blue/Timestep.hpp"

bool GpuThread::is_running()
{
	return _thread_running.load();
}

void GpuThread::run()
{
	_thread_running.store(true);
	_thread = std::thread(&GpuThread::render_thread_loop, this);
	blue::Context::logger().info("Started GPU thread.");
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

	blue::Context::window().init_gl_context();
	Timestep timestep(60, true);

	while (blue::Context::gpu_thread().is_running())
	{
		timestep.mark_start();

		renderer.clear();
		renderer.draw();

		window.swap_buffers();

		gpu_system.lock();
		gpu_system.execute();
		gpu_system.unlock();

		timestep.mark_end();
		timestep.delay();
	}
}
