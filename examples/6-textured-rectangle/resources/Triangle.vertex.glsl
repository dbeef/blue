layout(location = 0) in vec2 position;
layout(location = 1) in vec2 tex_coord;

layout(location = 4) uniform lowp mat4 model;


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
