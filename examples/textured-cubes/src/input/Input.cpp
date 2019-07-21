//
// Created by dbeef on 5/4/19.
//

#include "Application.h"
#include "Input.hpp"
#include "blue/Context.hpp"

void registerCallbacks(){

    const auto& camera = Application::instance().camera;
    const auto& rendering_lines = Application::instance().rendering_lines;
    const auto& place_block = Application::instance().place_block;
    const auto& remove_block = Application::instance().remove_block;
    const auto& destroying_blocks = Application::instance().destroying_blocks;

	auto w_callback = [camera] ()
	{
		const float CAMERA_SPEED = 0.5f;
		camera->cameraPos += camera->cameraFront * CAMERA_SPEED;
	};
	blue::Context::input().registerKeyCallback({w_callback, SDLK_w, SDL_KEYDOWN });
	
	auto a_callback = [camera] ()
	{
		const float CAMERA_SPEED = 0.5f;
		camera->cameraPos -= glm::normalize(glm::cross(camera->cameraFront, camera->CAMERA_UP)) * CAMERA_SPEED;
	};
	blue::Context::input().registerKeyCallback({ a_callback, SDLK_a, SDL_KEYDOWN });

	auto s_callback = [camera] ()
	{
		const float CAMERA_SPEED = 0.5f;
		camera->cameraPos -= camera->cameraFront * CAMERA_SPEED;
	};
	blue::Context::input().registerKeyCallback({ s_callback, SDLK_s, SDL_KEYDOWN });

	auto d_callback = [camera] ()
	{
		const float CAMERA_SPEED = 0.5f;
		camera->cameraPos += glm::normalize(glm::cross(camera->cameraFront, camera->CAMERA_UP)) * CAMERA_SPEED;
	};
	blue::Context::input().registerKeyCallback({ d_callback, SDLK_d, SDL_KEYDOWN });

	auto space_callback = [camera] ()
	{
		const float CAMERA_SPEED = 0.5f;
		auto camRight = glm::normalize(glm::cross(-glm::normalize(camera->cameraFront), camera->CAMERA_UP));
		auto camUp = glm::normalize(glm::cross(-glm::normalize(camera->cameraFront), camRight));
		camera->cameraPos += -camUp * CAMERA_SPEED;
	};
	blue::Context::input().registerKeyCallback({ space_callback, SDLK_SPACE, SDL_KEYDOWN });

	auto mouse_left_callback = [destroying_blocks, place_block, remove_block] ()
	{
		if (destroying_blocks->load())
			remove_block->store(true);
		else
			place_block->store(true);
	};
	blue::Context::input().registerMouseKeyCallback({ mouse_left_callback, SDL_BUTTON_LEFT, SDL_EventType::SDL_MOUSEBUTTONDOWN });

	auto mouse_right_callback = [destroying_blocks] ()
	{
		destroying_blocks->store(!destroying_blocks->load());
	};
	blue::Context::input().registerMouseKeyCallback({ mouse_right_callback, SDL_BUTTON_RIGHT, SDL_EventType::SDL_MOUSEBUTTONDOWN });

	auto mouse_middle_press_callback = [] ()
	{
		blue::Context::input().cursor_attached.store(true);
		SDL_SetRelativeMouseMode(SDL_TRUE);
		SDL_SetWindowGrab(blue::Context::window().getWindow(), SDL_TRUE);
		SDL_WarpMouseInWindow(blue::Context::window().getWindow(), blue::Context::window().getWidth() / 2, blue::Context::window().getHeight() / 2);
	};
	blue::Context::input().registerMouseKeyCallback({ mouse_middle_press_callback, SDL_BUTTON_MIDDLE, SDL_EventType::SDL_MOUSEBUTTONDOWN });
	
	auto mouse_middle_release_callback = [] ()
	{
		blue::Context::input().cursor_attached.store(false);
		SDL_SetRelativeMouseMode(SDL_FALSE);
		SDL_SetWindowGrab(blue::Context::window().getWindow(), SDL_TRUE);
	};

	blue::Context::input().registerMouseKeyCallback({ mouse_middle_release_callback, SDL_BUTTON_MIDDLE, SDL_EventType::SDL_MOUSEBUTTONUP });

	auto mouse_move_callback = [camera] (double xpos, double ypos)
	{
		if (blue::Context::input().cursor_attached.load())
		{
			GLfloat xoffset = xpos - camera->lastX;
			GLfloat yoffset = camera->lastY - ypos; // Reversed since y-coordinates go from bottom to left

			GLfloat sensitivity = 0.25;    // Change this value to your liking
			xoffset *= sensitivity;
			yoffset *= sensitivity;

			camera->yaw += xoffset;
			camera->pitch += yoffset;

			// Make sure that when pitch is out of bounds, screen doesn't get flipped
			if (camera->pitch > 89.0f)
				camera->pitch = 89.0f;
			if (camera->pitch < -89.0f)
				camera->pitch = -89.0f;

			glm::vec3 front;
			front.x = cos(glm::radians(camera->yaw)) * cos(glm::radians(camera->pitch));
			front.y = sin(glm::radians(camera->pitch));
			front.z = sin(glm::radians(camera->yaw)) * cos(glm::radians(camera->pitch));
			camera->cameraFront = glm::normalize(front);
		}

		Application::instance().last_x = xpos;
		Application::instance().last_y = ypos;
	};
	blue::Context::input().registerMouseMoveCallback(mouse_move_callback);
}
