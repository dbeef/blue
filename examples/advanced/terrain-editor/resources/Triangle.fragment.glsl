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

// flat so it would not interpolate color
flat in lowp vec3 ColorRGB;

out vec4 color;

void main()
{
    color = vec4(ColorRGB, 1.0f);
}
