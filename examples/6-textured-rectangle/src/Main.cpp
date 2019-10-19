#include <blue/Context.hpp>
#include <blue/Timestep.hpp>
#include <blue/ShaderUtils.h>
#include <blue/TextureUtils.hpp>
#include <blue/camera/OrthographicCamera.hpp>

#include <atomic>

int main(int argc, char *argv[])
{
    blue::Context::init();
    blue::Context::window().create(800, 600);
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

    // Now render thread is running and waiting for commands to process,
    // leaving this thread only for CPU logics.

    // Issue the GPU thread with task of compiling shader program:

    auto compile_shader_entity = ShaderUtils::make_entity("resources/Triangle.vertex.glsl",
                                                          "resources/Triangle.fragment.glsl");
    auto shader = blue::Context::gpu_system().submit(compile_shader_entity).get();

    // Issue the GPU thread with task of uploading mesh:

    Vertices vertices =
            {
                    /* Vertex pos */ -1.0f, -1.0f, /* Tex coord */ 0.0f, 0.0f,
                    /* Vertex pos */ -1.0f, 1.0f,  /* Tex coord */ 0.0f, 1.0f,
                    /* Vertex pos */ 1.0f, 1.0f,   /* Tex coord */ 1.0f, 1.0f,
                    /* Vertex pos */ 1.0f, -1.0f,  /* Tex coord */ 1.0f, 0.0f,
            };

    Indices indices =
            {
                    0, 1, 2, 2, 3, 0
            };

    Attributes attributes =
            {
                    {ShaderAttribute::Type::VEC2, ShaderAttribute::Purpose::VERTEX_POSITION,    ShaderAttribute::Buffer::VERTEX},
                    {ShaderAttribute::Type::VEC2, ShaderAttribute::Purpose::TEXTURE_COORDINATE, ShaderAttribute::Buffer::VERTEX}
            };

    auto vertex_array = blue::Context::gpu_system().submit(CreateMeshEntity{vertices, indices, attributes, 6}).get();

    // Create environment

    auto environment = blue::Context::gpu_system().submit(CreateEnvironmentEntity{}).get();

    // Upload camera's matrices

    OrthographicCamera camera(OrthographicCamera::Mode::SCREEN_SPACE);

    blue::Context::gpu_system().submit(UpdateEnvironmentEntity_Projection{environment, camera.get_projection()});
    blue::Context::gpu_system().submit(UpdateEnvironmentEntity_View{environment, camera.get_view()});

    // Create texture
    auto create_texture_entity = ImageUtils::read("resources/blue.png");
    create_texture_entity.slot = 3; // Arbitrary value, just to show that slots can be changed.

    auto texture_future = blue::Context::gpu_system().submit(create_texture_entity);
    texture_future.wait();
    auto texture = texture_future.get();

    // Now set used texture slot in shader:
    blue::Context::gpu_system().submit(
            UpdateUniformVariableEntity{ShaderAttribute::Type::INT, &create_texture_entity.slot, shader, 9, ""});

    // Submit render command consisting of compiled shader, uploaded mesh and following geometry properties:

    RenderEntity entity;
    entity.position = {blue::Context::window().get_width() / 2, blue::Context::window().get_height() / 2, 0.0f};
    entity.shader = shader;
    entity.vertex_array = vertex_array;
    entity.scale = 100.0f;
    entity.rotation = glm::identity<glm::quat>();
    entity.environment = environment;
    entity.texture1 = texture;

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
