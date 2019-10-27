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

in vec3 vertex;
out vec4 color;

void main()
{
  color = vec4(0.2f, 0.5f, 1.0f, 0.6f);
}
