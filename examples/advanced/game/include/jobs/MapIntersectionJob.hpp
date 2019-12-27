#pragma once

#include <atomic>
#include <thread>

class MapIntersectionJob
{
public:
	
	void shutdown();

	void start();

private:

	void intersection_loop();

	std::thread _intersection_thread;
	std::atomic_bool _running{ false };
};
