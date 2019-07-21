#pragma once

#include <thread>
#include <atomic>

class GpuThread
{
public:

	bool is_running();

	void run();

	void stop();

private:

	void render_thread_loop();

	std::atomic_bool _thread_running{ false };
	std::thread _thread;
};
