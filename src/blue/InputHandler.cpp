#include "blue/InputHandler.h"
#include "blue/Context.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_sdl.h"

void InputHandler::poll()
{
    // FIXME: Clean this class. Add checking on event type before processing it.

    SDL_Event event;
	while (SDL_PollEvent(&event)) {

		int xpos = blue::Context::window().get_width() / 2;
		int ypos = blue::Context::window().get_height() / 2;

#if defined(BLUE_WINDOWS) || defined(BLUE_LINUX)
		bool processed = ImGui_ImplSDL2_ProcessEvent(&event);
#endif

//
		//if (event.type == SDL_EventType::SDL_MULTIGESTURE) {
		//	if (fabs(event.mgesture.dDist) > 0.002) {
		//		//Pinch open
		//		if (event.mgesture.dDist > 0) {
		//			camera->cameraPos += camera->cameraFront;
		//		}
		//		//Pinch close
		//		else {
		//			camera->cameraPos -= camera->cameraFront;
		//		}
		//	}
		//}
		//else {

		switch (event.type)
		{
		case SDL_EventType::SDL_KEYDOWN:
		case SDL_EventType::SDL_KEYUP:
		{
			keyboard_callback(event.key.keysym.sym, event.type);
			break;
		}
		}

		mouse_button_callback(event.button.button, event.type, event.motion.x, event.motion.y);

		if (event.type == SDL_EventType::SDL_MOUSEMOTION)
		{
			if (blue::Context::window().is_cursor_attached())
				mouse_callback(event.motion.xrel + xpos, event.motion.yrel + ypos);
			else
			{
				blue::Context::window().set_last_xy(event.motion.x, event.motion.y);
				mouse_callback(event.motion.x, event.motion.y);
			}
		}

		if(event.type == SDL_EventType::SDL_MULTIGESTURE)
        {
		    gestureCallback(event.mgesture);
        }

		//if (event.type == SDL_QUIT)
		//	exit = true;
		//if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE &&
		//	event.window.windowID == SDL_GetWindowID(blue::Context::window().getWindow()))
		//	exit = true;
	}
}

void InputHandler::mouse_button_callback(int key, int action, double x, double y)
{
	for (auto& callback : mouseButtonCommands)
		if (key == callback.key_type && action == callback.action)
			callback.callback(x, y);
}

void InputHandler::mouse_callback(double xpos, double ypos)
{
	for (auto& callback : mouseCommands)
	{
		callback(xpos, ypos);
	}
}

void InputHandler::keyboard_callback(int key, int action)
{
	for (auto& callback : keyboardKeyCommands)
		if (key == callback.key_type && action)
			callback.callback();
}
