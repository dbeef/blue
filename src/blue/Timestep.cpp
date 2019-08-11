#include "blue/Timestep.hpp"

#include <chrono>

void Timestep::delay() const
{
	const auto delta = std::chrono::duration_cast<std::chrono::microseconds>(_end - _start);

#ifndef NDEBUG
	if (_logging)
	{
		static double counter = 0;
		counter++;
		if (counter >= _fps)
		{
			blue::Context::logger().info("Spent {} microseconds in timestep.", delta.count());
			counter = 0;
		}
	}
#endif

	if (delta > _frequency)
	{
		// no delay
	}
	else
	{
		const auto sleep_time = _frequency - delta;
		std::this_thread::sleep_for(sleep_time);
	}
}
