#include <blue/Context.hpp>
#include <blue/Timestep.hpp>
#include <blue/ShaderUtils.h>
#include <blue/ModelLoader.h>
#include <blue/PerspectiveCamera.hpp>

#include <cmath>
#include <atomic>

int main(int argc, char* argv[])
{
	blue::Context::init();
	blue::Context::window().create(800, 600);

	// Register ESC key callback: 
	std::atomic_bool running{ true };

	blue::Context::input().registerKeyCallback({
		[&running]() { running = false; },
			SDLK_ESCAPE,
		}
	);

	blue::Context::gpu_thread().run();

	// Now render thread is running and waiting for commands to process,
	// leaving this thread only for CPU logics. 

	// Issue the GPU thread with task of compiling shader program:

	auto shader_source = ShaderSource("resources/Triangle.vertex.glsl", "resources/Triangle.fragment.glsl");
	auto shader_future = blue::Context::gpu_system().submit(CompileShaderEntity{ shader_source.vertex, shader_source.fragment });
	shader_future.wait();
	auto shader = shader_future.get();

	// Issue the GPU thread with task of uploading mesh:

	Attributes attributes =
	{
		{ ShaderAttribute::Type::VEC3, ShaderAttribute::Purpose::VERTEX_POSITION},
		{ ShaderAttribute::Type::VEC3, ShaderAttribute::Purpose::COLOR},
		{ ShaderAttribute::Type::VEC3, ShaderAttribute::Purpose::NORMAL}
	};

	// Simple model I created, that utilizes vertex painting.
	auto scene_ptr = models::load_scene("resources/PineTree.fbx");
	unsigned int vertex_counter = 0;
	auto vertices = models::parse_scene(scene_ptr, attributes, vertex_counter);

	auto vertex_array_future = blue::Context::gpu_system().submit(CreateMeshEntity{ vertices, {}, attributes, vertex_counter });
	vertex_array_future.wait();
	auto vertex_array = vertex_array_future.get();

	// Create environment

	auto environment_future = blue::Context::gpu_system().submit(CreateEnvironmentEntity{});
	environment_future.wait();
	auto environment = environment_future.get();

	PerspectiveCamera camera;

	blue::Context::gpu_system().submit(UpdateEnvironmentEntity_Projection{ environment, camera.get_projection() });
	blue::Context::gpu_system().submit(UpdateEnvironmentEntity_View{ environment, camera.get_view() });

	// WASD callback + mouse

	blue::Context::input().registerKeyCallback({
			[&running]() { running = false; },
			SDLK_ESCAPE,
			SDL_KEYDOWN
		}
	);

	auto w_callback = [&camera, &environment]()
	{
		const float CAMERA_SPEED = 0.5f;
		camera.cameraPos += camera.cameraFront * CAMERA_SPEED;
		blue::Context::gpu_system().submit(UpdateEnvironmentEntity_View{ environment, camera.get_view() });
	};
	blue::Context::input().registerKeyCallback({ w_callback, SDLK_w, SDL_KEYDOWN });

	auto a_callback = [&camera, &environment]()
	{
		const float CAMERA_SPEED = 0.5f;
		camera.cameraPos -= glm::normalize(glm::cross(camera.cameraFront, camera.CAMERA_UP)) * CAMERA_SPEED;
		blue::Context::gpu_system().submit(UpdateEnvironmentEntity_View{ environment, camera.get_view() });
	};
	blue::Context::input().registerKeyCallback({ a_callback, SDLK_a, SDL_KEYDOWN });

	auto s_callback = [&camera, &environment]()
	{
		const float CAMERA_SPEED = 0.5f;
		camera.cameraPos -= camera.cameraFront * CAMERA_SPEED;
		blue::Context::gpu_system().submit(UpdateEnvironmentEntity_View{ environment, camera.get_view() });
	};

	blue::Context::input().registerKeyCallback({ s_callback, SDLK_s, SDL_KEYDOWN });

	auto d_callback = [&camera, &environment]()
	{
		const float CAMERA_SPEED = 0.5f;
		camera.cameraPos += glm::normalize(glm::cross(camera.cameraFront, camera.CAMERA_UP)) * CAMERA_SPEED;
		blue::Context::gpu_system().submit(UpdateEnvironmentEntity_View{ environment, camera.get_view() });
	};
	blue::Context::input().registerKeyCallback({ d_callback, SDLK_d, SDL_KEYDOWN });

	auto mouse_move_callback = [&camera, &environment](double xpos, double ypos)
	{
		GLfloat xoffset = xpos - camera.lastX;
		GLfloat yoffset = camera.lastY - ypos; // Reversed since y-coordinates go from bottom to left

		GLfloat sensitivity = 0.25;    // Change this value to your liking
		xoffset *= sensitivity;
		yoffset *= sensitivity;

		camera.yaw += xoffset;
		camera.pitch += yoffset;

		// Make sure that when pitch is out of bounds, screen doesn't get flipped
		if (camera.pitch > 89.0f)
			camera.pitch = 89.0f;
		if (camera.pitch < -89.0f)
			camera.pitch = -89.0f;

		glm::vec3 front;
		front.x = cos(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch));
		front.y = sin(glm::radians(camera.pitch));
		front.z = sin(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch));
		camera.cameraFront = glm::normalize(front);

		blue::Context::gpu_system().submit(UpdateEnvironmentEntity_View{ environment, camera.get_view() });
	};

	blue::Context::input().registerMouseMoveCallback(mouse_move_callback);

	// Submit render command consisting of compiled shader and uploaded mesh

	RenderEntity entity;
	entity.position = { 0, 0, -2.5f };
	entity.shader = shader;
	entity.vertex_array = vertex_array;
	entity.scale = 0.5f;
	entity.rotation = { 0, 0, 0, 0 };
	entity.environment = environment;

	RenderEntityId id = blue::Context::renderer().add(entity);

	// Start logics loop with timestep limited to 30 times per second:
	Timestep timestep(30);

	while (running)
	{
		timestep.mark_start();
		blue::Context::input().poll();
		timestep.mark_end();
		timestep.delay();
	}

	blue::Context::gpu_thread().stop();
	blue::Context::dispose();

	return 0;
}
