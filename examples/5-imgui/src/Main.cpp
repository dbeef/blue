// TODO: Actually provide Imgui sample

#include <blue/Context.hpp>
#include <blue/Timestep.hpp>

#include <cmath>
#include <atomic>

int main(int argc, char* argv[])
{
	blue::Context::init();
	blue::Context::window().create(800, 600);
	blue::Context::gpu_thread().run();

	// Now render thread is running and waiting for commands to process,
	// leaving this thread only for CPU logics. 

	// Register ESC key callback: 
	std::atomic_bool running{ true };

	blue::Context::input().registerKeyCallback({
			[&running]() { running = false; },
			SDLK_ESCAPE,
			SDL_KEYDOWN
		}
	);

	// Start a loop with timestep limited to 30 times per second:
	Timestep timestep(30);

	float red = 0.0f;
	while (running)
	{
		timestep.mark_start();
		red += 0.01;
		blue::Context::input().poll();
		blue::Context::gpu_system().submit(SetClearColorEntity{ glm::vec3(std::sin(red), 0.5f, 0.3f) });
		timestep.mark_end();
		timestep.delay();
	}

	blue::Context::gpu_thread().stop();
	blue::Context::dispose();

	return 0;
}
