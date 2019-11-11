#pragma once

#include <thread>
#include <atomic>
#include <future>
#include <condition_variable>

class GpuThread
{
public:

	void wait_for_render_pass();

	bool is_running();

	bool run();

	void stop();

	inline std::uint64_t get_time_spent() { return _time_spent.load(); }

private:

	void render_thread_loop();

	std::atomic<std::uint64_t> _time_spent{0};
	std::promise<bool> _start_thread_status;
	std::atomic_bool _thread_running{ false };
	std::thread _thread;
	std::mutex render_pass_mtx;
	std::condition_variable render_pass_cv;
};
