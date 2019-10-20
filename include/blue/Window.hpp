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

		bool create(std::uint16_t width, std::uint16_t height);

		bool create_fullscreen();

		bool create_hidden();

		bool init_gl_context();

		inline std::uint16_t get_width() const { return _width.load(); }

		inline std::uint16_t get_height() const { return _height.load(); }

		inline SDL_GLContext get_context() const { return _gl_context.load(); }

		inline SDL_Window* get_window() const { return _window_handle.load(); }

		inline void swap_buffers() const { SDL_GL_SwapWindow(_window_handle.load()); }

		inline bool is_cursor_attached() const { return _cursor_attached.load(); }

		void attach_cursor();

		void detach_cursor();

		std::uint16_t get_last_x();

		std::uint16_t get_last_y();

		void set_last_xy(std::uint16_t x, std::uint16_t y);

	private:

		bool _create();

		void update_size();

		std::atomic_bool _cursor_attached{ true };
		std::atomic<SDL_GLContext> _gl_context{};
		std::atomic<SDL_Window*> _window_handle{};
		std::string _title = "Blue";
		std::atomic<std::uint16_t> _width{ 0 };
		std::atomic<std::uint16_t> _height{ 0 };
		std::atomic<std::uint16_t> _last_x{ 0 };
		std::atomic<std::uint16_t> _last_y{ 0 };
		bool _fullscreen = false;
		bool _hidden = false;
	};
}
