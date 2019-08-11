layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;

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


out vec3 out_color;

void main()
{
//        gl_Position = vec4(position.x, position.y, position.z, 1.0);
        gl_Position = projection * view * model * vec4(position, 1.0f);
//        gl_Position = model * vec4(position, 1.0f);
//        gl_Position = projection * model * vec4(position, 1.0f);
        out_color = color;
}
