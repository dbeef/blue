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

in vec3 Normal;
in vec2 TextureCoordinates;
in vec3 FragPos;
out vec4 color;

void main()
{
    vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);
    vec3 ambient = ambientStrength * lightColor;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    color = vec4(ambient + diffuse, 1.0f);
}
