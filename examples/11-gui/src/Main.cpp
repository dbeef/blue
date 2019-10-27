#include <blue/Context.hpp>
#include <blue/Timestep.hpp>
#include <blue/ShaderUtils.h>
#include <blue/TextureUtils.hpp>
#include <blue/camera/OrthographicCamera.hpp>
#include <blue/gui/Button.hpp>

#include <atomic>

int main(int argc, char *argv[])
{
    blue::Context::init();
    blue::Context::window().create(512, 512);
    blue::Context::gpu_thread().run();

    // Register ESC key callback:
    std::atomic_bool running{true};

    blue::Context::input().registerKeyCallback({
                                                       [&running]()
                                                       { running = false; },
                                                       SDLK_ESCAPE,
                                                       SDL_KEYDOWN
                                               }
    );

    OrthographicCamera camera(OrthographicCamera::Mode::SCREEN_SPACE, blue::Context::window().get_width(), blue::Context::window().get_height());

    auto environment = blue::Context::gpu_system().submit(CreateEnvironmentEntity{}).get();
    blue::Context::gpu_system().submit(UpdateEnvironmentEntity_Projection{environment, camera.get_projection()});
    blue::Context::gpu_system().submit(UpdateEnvironmentEntity_View{environment, camera.get_view()});

    auto clicked_entity = CreateTextureEntity{ImageUtils::read("resources/clicked.png")};
    clicked_entity.slot = 0;
    auto texture_clicked = blue::Context::gpu_system().submit(clicked_entity).get();

    auto idle_entity = CreateTextureEntity{ImageUtils::read("resources/idle.png")};
    idle_entity.slot = 1;
    auto texture_idle = blue::Context::gpu_system().submit(idle_entity).get();

    Button::init();
    Button button;
    PlacingRules rules;
    rules.width_in_screen_percent = 10;
    rules.height_in_screen_percent = 10;
    rules.x_in_screen_percent_from_left = 80;
    rules.y_in_screen_percent_from_up = 20;
    button.update_placing_rules(rules);
    button.create(environment, texture_clicked, texture_idle);

    MouseKeyCallback down_callback;
    down_callback.key_type = SDL_BUTTON_LEFT;
    down_callback.action = SDL_MOUSEBUTTONDOWN;
    down_callback.callback = [&button](double x, double y) {
        bool clicked = button.is_clicked(x, y);
        if(clicked) button.set_clicked(true);
    };

    MouseKeyCallback release_callback;
    release_callback.key_type = SDL_BUTTON_LEFT;
    release_callback.action = SDL_MOUSEBUTTONUP;
    release_callback.callback = [&button](double x, double y) {
        button.set_clicked(false);
    };

    blue::Context::input().registerMouseKeyCallback({down_callback});
    blue::Context::input().registerMouseKeyCallback({release_callback});

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
