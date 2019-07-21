layout (location = 0) in vec3 aPos;

layout(location = 4) uniform mat4 model;

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

void main()
{
    gl_Position = lightSpaceMatrix * model * vec4(aPos, 1.0);
}