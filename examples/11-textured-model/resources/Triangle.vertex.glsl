layout(location = 0) in vec3 position;
layout(location = 1) in vec2 tex_coordinates;
layout(location = 2) in vec3 m_ambient;
layout(location = 3) in vec3 m_diffuse;
layout(location = 4) in vec3 m_specular;
layout(location = 5) in float m_shininess;
layout(location = 6) in vec3 normal;

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

out vec3 FragPos;
out vec2 TexCoordinates;
out vec3 Normal;

out vec3 MaterialAmbient;
out vec3 MaterialDiffuse;
out vec3 MaterialSpecular;
out float MaterialShininess;

void main()
{
        gl_Position = projection * view * model * vec4(position, 1.0f);
        // Forwarding values to fragment shader
        FragPos = vec3(model * vec4(position, 1.0f));
        Normal = -1.0 * vec3(model * vec4(normal, 1.0f));
        TexCoordinates = tex_coordinates;
        MaterialAmbient = m_ambient;
        MaterialDiffuse = m_diffuse;
        MaterialSpecular = m_specular;
        MaterialShininess = m_shininess;
}
