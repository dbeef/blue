#pragma once

#include <atomic>
#include <thread>
#include <blue/Renderer.h>

class FramerateRenderingJob
{
public:

    void shutdown();

    void start();

private:

    void framerate_rendering_loop();

	RenderEntity _entity;
    std::thread _intersection_thread;
    std::atomic_bool _running{ false };
};
