layout(location = 0) in vec3 position;
layout(location = 1) in vec2 tex_coord;

layout(location = 4) uniform lowp mat4 model;
//layout(location = 5) uniform lowp mat4 view;
//layout(location = 6) uniform lowp mat4 projection;


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
//        gl_Position = vec4(position.x, position.y, position.z, 1.0);
        gl_Position = projection * view * model * vec4(position, 1.0f);
        TexCoord = tex_coord;
}
