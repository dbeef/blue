layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 color;
layout(location = 3) in mat4 instanced_model;

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

flat out lowp vec3 ColorRGB;
out vec3 Normal;
out vec3 FragPos;
out lowp vec4 FragPosLightSpace;

void main()
{
        gl_Position = projection * view * model * instanced_model * vec4(position, 1.0f);
        // Forwarding values to fragment shader
        ColorRGB = color;
        Normal = normal;
        FragPos = vec3(model * instanced_model * vec4(position, 1.0f));
        FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);
}


