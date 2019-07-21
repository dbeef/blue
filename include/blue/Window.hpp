#pragma once

#include <atomic>
#include <string>

#include <SDL2/SDL.h>

namespace blue
{
	class Window
	{
	public:

		~Window();

		bool create();

		bool create(std::uint16_t width, std::uint16_t height);

		bool init_gl_context();

		inline std::uint16_t get_width() { return _width.load(); }

		inline std::uint16_t get_height() { return _height.load(); }

		inline void swap_buffers() const { SDL_GL_SwapWindow(_window_handle); }

	private:

		bool _create();

		void update_size();

		SDL_GLContext _gl_context{};
		SDL_Window* _window_handle{};
		std::string _title = "Blue";
		std::atomic_uint16_t _width{ 0 };
		std::atomic_uint16_t _height{ 0 };
		bool _fullscreen = false;
	};
}
