layout (location = 0) in vec3 aPos;

layout(location = 0) uniform mat4 model;

layout (std140) uniform Matrices
{
// MVP
    mat4 view;
    mat4 projection;
// Light
    float ambientStrength;
    vec3 lightColor;
    vec3 lightPos;
    vec3 cameraPos;
// Shadows
    mat4 lightSpaceMatrix;
};

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
