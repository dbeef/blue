#include <Callbacks.hpp>
#include <blue/Assertions.h>
#include <blue/Context.hpp>
#include <SDL_events.h>
#include <Game.hpp>
#include <Resources.hpp>

static const float CAMERA_SPEED = 0.25f;

Callbacks* Callbacks::_instance = nullptr;

Callbacks &Callbacks::instance()
{
    BLUE_ASSERT(_instance);
    return *_instance;
}

void Callbacks::init()
{
    BLUE_ASSERT(!_instance);
    _instance = new Callbacks();
}

void Callbacks::dispose()
{
    BLUE_ASSERT(_instance);
    delete _instance;
    _instance = nullptr;
}

void Callbacks::register_callbacks()
{
    blue::Context::input().registerKeyCallback({
                                                       []() { Game::instance().shutdown(); },
                                                       SDLK_ESCAPE,
                                                       SDL_KEYDOWN
                                               }
    );

    auto w_callback = [this]()
    {
        auto& map_environment = Resources::instance().map_environment;
        map_environment.camera.go_forward(CAMERA_SPEED);
        blue::Context::gpu_system().submit(UpdateEnvironmentEntity_View{ map_environment.environment, map_environment.camera.get_view() });
        blue::Context::gpu_system().submit(UpdateEnvironmentEntity_CameraPos{ map_environment.environment, map_environment.camera.get_position() });
    };
    blue::Context::input().registerKeyCallback({ w_callback, SDLK_w, SDL_KEYDOWN });

    auto mouse_middle_callback = [this](double x, double y)
    {
        if (blue::Context::window().is_cursor_attached())
        {
            blue::Context::window().detach_cursor();
        }
        else
        {
            blue::Context::window().attach_cursor();
        }
    };
    blue::Context::input().registerMouseKeyCallback({ mouse_middle_callback, SDL_BUTTON_MIDDLE, SDL_MOUSEBUTTONDOWN });

    auto mouse_left_press_callback = [this](double xpos, double ypos)
    {
        blue::Context::logger().info("Press on: {} {}", xpos, ypos);
        Game::instance().input.last_x = xpos;
        Game::instance().input.last_y = ypos;
        Game::instance().input.gesture.store(true);
    };
    blue::Context::input().registerMouseKeyCallback({ mouse_left_press_callback, SDL_BUTTON_LEFT, SDL_MOUSEBUTTONDOWN });

    auto mouse_left_release_callback = [this](double xpos, double ypos)
    {
        blue::Context::logger().info("Release on: {} {}", xpos, ypos);
        Game::instance().input.last_x = xpos;
        Game::instance().input.last_y = ypos;
        Game::instance().input.gesture.store(false);
    };
    blue::Context::input().registerMouseKeyCallback({ mouse_left_release_callback, SDL_BUTTON_LEFT, SDL_MOUSEBUTTONUP });

    auto s_callback = [this]()
    {
        auto& map_environment = Resources::instance().map_environment;
        map_environment.camera.go_backward(CAMERA_SPEED);
        blue::Context::gpu_system().submit(UpdateEnvironmentEntity_View{ map_environment.environment, map_environment.camera.get_view() });
        blue::Context::gpu_system().submit(UpdateEnvironmentEntity_CameraPos{ map_environment.environment, map_environment.camera.get_position() });
    };
    blue::Context::input().registerKeyCallback({ s_callback, SDLK_s, SDL_KEYDOWN });

    auto a_callback = [this]()
    {
        auto& map_environment = Resources::instance().map_environment;
        map_environment.camera.go_left(CAMERA_SPEED);
        blue::Context::gpu_system().submit(UpdateEnvironmentEntity_View{ map_environment.environment, map_environment.camera.get_view() });
        blue::Context::gpu_system().submit(UpdateEnvironmentEntity_CameraPos{ map_environment.environment, map_environment.camera.get_position() });
    };
    blue::Context::input().registerKeyCallback({ a_callback, SDLK_a, SDL_KEYDOWN });

    auto d_callback = [this]()
    {
        auto& map_environment = Resources::instance().map_environment;
        map_environment.camera.go_right(CAMERA_SPEED);
        blue::Context::gpu_system().submit(UpdateEnvironmentEntity_View{ map_environment.environment, map_environment.camera.get_view() });
        blue::Context::gpu_system().submit(UpdateEnvironmentEntity_CameraPos{ map_environment.environment, map_environment.camera.get_position() });
    };
    blue::Context::input().registerKeyCallback({ d_callback, SDLK_d, SDL_KEYDOWN });

    auto mouse_move_callback = [this](double xpos, double ypos)
    {
        auto& map_environment = Resources::instance().map_environment;

        if (blue::Context::window().is_cursor_attached())
        {
            map_environment.camera.mouse_rotation(xpos, ypos);
        }

        blue::Context::gpu_system().submit(UpdateEnvironmentEntity_View{ map_environment.environment, map_environment.camera.get_view() });
        blue::Context::gpu_system().submit(UpdateEnvironmentEntity_CameraPos{ map_environment.environment, map_environment.camera.get_position() });

        if (Game::instance().input.gesture.load())
        {
            glm::vec2 origin = { Game::instance().input.last_x.load() , Game::instance().input.last_y.load() };
            glm::vec2 target = { xpos, ypos };
            glm::vec2 delta = glm::normalize(origin - target);

            auto pos = map_environment.camera.get_position();
            pos.x += 0.2f * delta.x;
            pos.z += 0.2f * delta.y;
            map_environment.camera.set_pos(pos);

            blue::Context::gpu_system().submit(UpdateEnvironmentEntity_View{ map_environment.environment, map_environment.camera.get_view() });
            blue::Context::gpu_system().submit(UpdateEnvironmentEntity_CameraPos{ map_environment.environment, map_environment.camera.get_position() });

            Game::instance().input.last_x = xpos;
            Game::instance().input.last_y = ypos;
        }

        const auto& camera = Resources::instance().map_environment.camera;
    };

    blue::Context::input().registerMouseMoveCallback(mouse_move_callback);

    auto gesture_callback = [this](SDL_MultiGestureEvent event)
    {
        const float& delta_rotation_rad = event.dTheta;
        auto& map_environment = Resources::instance().map_environment;
        map_environment.camera.add_rotation(delta_rotation_rad * 10, 0);
        blue::Context::gpu_system().submit(UpdateEnvironmentEntity_View{ map_environment.environment, map_environment.camera.get_view() });
    };

    blue::Context::input().registerGestureCallback(gesture_callback);
}
