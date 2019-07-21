#pragma once

#include "blue/Logger.hpp"
#include "blue/Window.hpp"
#include "blue/Renderer.h"
#include "blue/InputHandler.h"
#include "blue/GpuThread.hpp"
#include "blue/gpu/GpuCommandSystem.hpp"

namespace blue
{
	class Context {

	public:

		Context() {}
		
		~Context()
		{
			blue::Context::logger().info("Disposed context.");
		}

		inline static void init()
		{
			context = new Context();
			blue::Context::logger().info("Created context.");
		}

		inline static void dispose()
		{
			delete context;
			context = nullptr;
		}

		inline static spdlog::logger& logger()
		{
			return context->_logger.get();
		}

		inline static Window& window()
		{
			return context->_window;
		}

		inline static Renderer& renderer()
		{
			return context->_renderer;
		}
		
		inline static InputHandler& input()
		{
			return context->_input;
		}

		inline static GpuCommandSystem& gpu_system()
		{
			return context->_gpu_command_system;
		}

		inline static GpuThread& gpu_thread()
		{
			return context->_gpu_thread;
		}
		
	private:

		Logger _logger;
		Window _window;
		Renderer _renderer;
		InputHandler _input;
		GpuThread _gpu_thread;
		GpuCommandSystem _gpu_command_system;

		static Context* context;
	};
}
