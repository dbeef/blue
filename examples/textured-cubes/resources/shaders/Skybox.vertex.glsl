layout (location = 0) in vec3 position;
// not used
layout(location = 1) in lowp vec2 texCoord;
layout(location = 2) in lowp float layer;
layout(location = 3) in lowp vec3 normal;

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

out vec3 TexCoords;

void main()
{
    gl_Position = projection * view * vec4(position, 1.0);
    TexCoords = position;
}
