//
// Created by dbeef on 4/4/19.
//

#ifndef OPENGL_PLAYGROUND_INPUTHANDLER_H
#define OPENGL_PLAYGROUND_INPUTHANDLER_H

#include <SDL2/SDL.h>
#include <vector>
#include <utility>
#include <functional>
#include <atomic>
#include <algorithm>

// Currently using SDL's enums for keys / actions (press/release).
// TODO: Map SDL's enums to Blue's enums to have a layer of abstraction from SDL.
using MouseCallback = std::function<void(double dx, double dy)>;

struct KeyCallback
{
	std::function<void()> callback;
	int key_type;
	int action;
};

// TODO: Rename this class to "Input".
class InputHandler {

public:

	inline void registerKeyCallback(const KeyCallback& callback)
	{
		auto iterator = std::remove_if(keyboardKeyCommands.begin(), keyboardKeyCommands.end(), [callback](const KeyCallback& list_callback) -> bool {
			return callback.key_type == list_callback.key_type && callback.action == list_callback.action;
			});

		if (iterator != keyboardKeyCommands.end())
		{
			keyboardKeyCommands.erase(iterator);
		}

		keyboardKeyCommands.emplace_back(callback);
	}
	
	inline void registerMouseKeyCallback(const KeyCallback& callback)
	{
		auto iterator = std::remove_if(mouseButtonCommands.begin(), mouseButtonCommands.end(), [callback](const KeyCallback& list_callback) -> bool {
			return callback.key_type == list_callback.key_type && callback.action == list_callback.action;
			});

		if (iterator != mouseButtonCommands.end())
		{
			mouseButtonCommands.erase(iterator);
		}

		mouseButtonCommands.emplace_back(callback);
	}

	inline void registerMouseMoveCallback(const MouseCallback& callback)
	{
		mouseCommands.emplace_back(callback);
	}

	void poll();

	void mouse_callback(double xpos, double ypos);

	void keyboard_callback(int key, int action);

	void mouse_button_callback(int key, int action);

	std::vector<KeyCallback> mouseButtonCommands;
	std::vector<KeyCallback> keyboardKeyCommands;
	std::vector<MouseCallback> mouseCommands;
};

#endif //OPENGL_PLAYGROUND_INPUTHANDLER_H
