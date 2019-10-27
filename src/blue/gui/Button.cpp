#include <blue/gui/Button.hpp>
#include <blue/Context.hpp>
#include <blue/ShaderUtils.h>

#include <string>
#include <atomic>

namespace
{
    // Generic texture rendering shader, utilizing
    // slot 0 for clicked and slot 1 for idle texture.
    ShaderId shader_program_clicked{};
    ShaderId shader_program_idle{};
    VertexArray vertex_array{};
    std::atomic<bool> initialized{false};
}

void Button::update_placing_rules(const PlacingRules &rules)
{
    _rules = rules;
    _rules.evaluate(_x_center, _y_center, _width, _height);
}

void Button::create(const UniformBufferId &environment, const Texture &clicked, const Texture &idle)
{
    BLUE_ASSERT(initialized.load());

    _entity.position = {_x_center, _y_center, 0.0f};

    if (_clicked)
    {
        _entity.shader = shader_program_clicked;
    }
    else
    {
        _entity.shader = shader_program_idle;
    }

    _entity.vertex_array = vertex_array;
    _entity.scale = {_width, _height, 1.0f};
    _entity.rotation = glm::identity<glm::quat>();
    _entity.environment = environment;
    _entity.textures[0] = clicked;
    _entity.textures[1] = idle;

    _entity.id = blue::Context::renderer().add(_entity);
}

void Button::set_clicked(bool clicked)
{
    if (_clicked != clicked)
    {
        if (clicked)
        {
            _entity.shader = shader_program_clicked;
        }
        else
        {
            _entity.shader = shader_program_idle;
        }
        blue::Context::renderer().update(_entity);
    }
    _clicked = clicked;
}

void Button::init()
{
    const std::string vertex_shader =
            R"(
                layout(location = 0) in vec2 position;
                layout(location = 1) in vec2 tex_coord;

                layout(location = 0) uniform lowp mat4 model;

                layout (std140) uniform Matrices
                {
                // MVP
                        mat4 view;
                        mat4 projection;
                // Light
                        float ambientStrength;
                        vec3 lightColor;
                        vec3 lightPos;
                // Shadows
                        mat4 lightSpaceMatrix;
                };

                out vec2 TexCoord;

                void main()
                {
                        gl_Position = projection * view * model * vec4(position, 0.0f, 1.0f);
                        TexCoord = tex_coord;
                }
            )";

    const std::string fragment_shader_clicked =
            R"(
                layout(location = 9) uniform sampler2D sampler_clicked;
                in vec2 TexCoord;
                out vec4 color;

                void main()
                {
                        color = texture(sampler_clicked, TexCoord);
                }
            )";

    const std::string fragment_shader_idle =
            R"(
                layout(location = 9) uniform sampler2D sampler_idle;
                in vec2 TexCoord;
                out vec4 color;

                void main()
                {
                        color = texture(sampler_idle, TexCoord);
                }
            )";

    shader_program_clicked = blue::Context::gpu_system().submit(
            CompileShaderEntity{vertex_shader, fragment_shader_clicked}).get();
    shader_program_idle = blue::Context::gpu_system().submit(
            CompileShaderEntity{vertex_shader, fragment_shader_idle}).get();

    Vertices vertices =
            {
                    /* Vertex pos */ -0.5f, -0.5f, /* Tex coord */ 0.0f, 0.0f,
                    /* Vertex pos */ -0.5f, 0.5f,  /* Tex coord */ 0.0f, 1.0f,
                    /* Vertex pos */ 0.5f, 0.5f,   /* Tex coord */ 1.0f, 1.0f,
                    /* Vertex pos */ 0.5f, -0.5f,  /* Tex coord */ 1.0f, 0.0f,
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

    vertex_array = blue::Context::gpu_system().submit(CreateMeshEntity{vertices, indices, attributes, 6}).get();

    GLint clicked_slot = 0;
    GLint idle_slot = 1;

    blue::Context::gpu_system().submit(
            UpdateUniformVariableEntity{ShaderAttribute::Type::INT, &clicked_slot, shader_program_clicked, 9, ""});
    blue::Context::gpu_system().submit(
            UpdateUniformVariableEntity{ShaderAttribute::Type::INT, &idle_slot, shader_program_idle, 9, ""});
    blue::Context::gpu_thread().wait_for_render_pass();

    initialized.store(true);
}

bool Button::is_clicked(std::uint16_t click_x, std::uint16_t click_y) const
{
    const bool x_condition = click_x > _x_center - (_width / 2) && click_x < _x_center + (_width / 2);
    const bool y_condition = click_y > _y_center - (_height / 2)&& click_y < _y_center + (_height / 2);
    return x_condition && y_condition;
}
