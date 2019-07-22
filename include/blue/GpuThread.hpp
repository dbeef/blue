#pragma once

#include <thread>
#include <atomic>
#include <future>

class GpuThread
{
public:

	bool is_running();

	bool run();

	void stop();

private:

	void render_thread_loop();

	std::promise<bool> _start_thread_status;
	std::atomic_bool _thread_running{ false };
	std::thread _thread;
};
