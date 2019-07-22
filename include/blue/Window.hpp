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

		inline std::uint16_t get_width() const { return _width.load(); }

		inline std::uint16_t get_height() const { return _height.load(); }

		inline SDL_GLContext get_context() const { return _gl_context.load(); }

		inline SDL_Window* get_window() const { return _window_handle.load(); }

		inline void swap_buffers() const { SDL_GL_SwapWindow(_window_handle.load()); }

		inline bool is_cursor_attached() const { return _cursor_attached.load(); }

		inline void attach_cursor()
		{
			_cursor_attached.store(false);
			SDL_SetRelativeMouseMode(SDL_TRUE);
			SDL_SetWindowGrab(get_window(), SDL_TRUE);
			SDL_WarpMouseInWindow(get_window(), get_width() / 2, get_height() / 2);
		}

		inline void detach_cursor()
		{
			_cursor_attached.store(true);
			SDL_SetRelativeMouseMode(SDL_FALSE);
			SDL_SetWindowGrab(get_window(), SDL_TRUE);
		}

	private:

		bool _create();

		void update_size();

		std::atomic_bool _cursor_attached{ true };
		std::atomic<SDL_GLContext> _gl_context{};
		std::atomic<SDL_Window*> _window_handle{};
		std::string _title = "Blue";
		std::atomic_uint16_t _width{ 0 };
		std::atomic_uint16_t _height{ 0 };
		bool _fullscreen = false;
	};
}
