#include "blue/Window.hpp"
#include "blue/Context.hpp"
#include "blue/Assertions.h"

#include "glad/glad.h"
#ifdef BLUE_ANDROID
#	include <glad/glad_egl.h>
#endif

namespace 
{
	void set_window_attributes()
	{
#ifdef BLUE_ANDROID
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
#else
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
#endif
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	}
}

namespace blue
{
	Window::~Window()
	{
		SDL_GL_DeleteContext(_gl_context);
		SDL_DestroyWindow(_window_handle);
	}

	void Window::update_size()
	{
		int display_w, display_h;
		SDL_GetWindowSize(_window_handle, &display_w, &display_h);
		_width = display_w;
		_height = display_h;
	}

	bool Window::create_fullscreen()
	{
		_fullscreen = true;
		return _create();
	}

	bool Window::create(std::uint16_t width, std::uint16_t height)
	{
		_fullscreen = false;
		_width = width;
		_height = height;
		return _create();
	}
	
	bool Window::create_hidden()
	{
		_fullscreen = false;
		// Leaving it as 1x1 window, as zero width/height may lead to
		// RenderDoc not being able to inject into process.
		_width = 1;
		_height = 1;
		_hidden = true;
		return _create();
	}

	bool Window::_create()
	{
		SDL_Init(SDL_INIT_VIDEO);
		set_window_attributes();

		Uint32 flags = SDL_WINDOW_OPENGL;
		if (_fullscreen)
		{
			flags = flags | SDL_WINDOW_FULLSCREEN_DESKTOP;
		}
		
		if (_hidden)
		{
			flags = flags | SDL_WINDOW_HIDDEN;
		}

		_window_handle = SDL_CreateWindow(
			_title.c_str(),
			SDL_WINDOWPOS_UNDEFINED, // initial x position
			SDL_WINDOWPOS_UNDEFINED, // initial y position
			_width,
			_height,
			flags
		);

		if (_window_handle == nullptr)
		{
			blue::Context::logger().info("Failed to create window");
			return false;
		}

		// Size may have changed if fullscreen was specified.
		update_size();
		blue::Context::logger().info("Created window.");
		return true;
	}

	bool Window::init_gl_context()
	{
		_gl_context = SDL_GL_CreateContext(_window_handle);
		SDL_GL_MakeCurrent(_window_handle, _gl_context);

#ifdef BLUE_ANDROID
		gladLoadGLES2Loader((GLADloadproc)SDL_GL_GetProcAddress);
#else
		assert(gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress));
#endif

		// Enable blending
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		DebugGlCall(glEnable(GL_DEPTH_TEST));
		DebugGlCall(glEnable(GL_STENCIL_TEST));
		DebugGlCall(glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE));
		DebugGlCall(glViewport(0, 0, get_width(), get_height()));

		return true;
	}

	void Window::attach_cursor()
	{
		_cursor_attached.store(true);
		SDL_SetRelativeMouseMode(SDL_TRUE);
		SDL_SetWindowGrab(get_window(), SDL_TRUE);
		SDL_WarpMouseInWindow(get_window(), get_width() / 2, get_height() / 2);
	}

	void Window::detach_cursor()
	{
		_cursor_attached.store(false);
		SDL_SetRelativeMouseMode(SDL_FALSE);
		SDL_SetWindowGrab(get_window(), SDL_TRUE);
	}

	std::uint16_t Window::get_last_x()
	{
		return _last_x.load();
	}

	std::uint16_t Window::get_last_y()
	{
		return _last_y.load();
	}

	void Window::set_last_xy(std::uint16_t x, std::uint16_t y)
	{
		_last_x.store(x);
		_last_y.store(y);
	}
}
