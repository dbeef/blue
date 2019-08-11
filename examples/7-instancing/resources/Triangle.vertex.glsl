layout(location = 0) in vec3 position;
layout(location = 1) in vec3 translation;

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
        gl_Position = projection * view * model * vec4(position + translation, 1.0f);
        out_color = vec3(float(gl_InstanceID) / 100, float(gl_InstanceID) / 100, float(gl_InstanceID) / 100);
}
