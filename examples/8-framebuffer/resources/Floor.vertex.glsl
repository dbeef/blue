layout(location = 0) in vec3 position;
layout(location = 1) in vec2 coordinates;
layout(location = 2) in vec3 normal;

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
        vec3 cameraPos;
// Shadows
        mat4 lightSpaceMatrix;
};

out vec2 Coordinates;
out vec3 Normal;
out vec3 FragPos;
out lowp vec4 FragPosLightSpace;

void main()
{
        gl_Position = projection * view * model * vec4(position, 1.0f);
        // Forwarding values to fragment shader
        Coordinates = coordinates;
        Normal = normal;
        FragPos = vec3(model * vec4(position, 1.0f));
        FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);
}


