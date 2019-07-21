// From: http://codeflow.org/entries/2012/aug/02/easy-wireframe-display-with-barycentric-coordinates/

layout(location = 0) in lowp vec3 position;
layout(location = 1) in lowp vec2 color_rg;
layout(location = 2) in lowp float color_b;
layout(location = 3) in lowp vec3 barycentric;

layout(location = 4) uniform lowp mat4 model;

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


out lowp vec3 _barycentric;

void main(){
    gl_Position = projection * view * model * vec4(position, 1.0f);
    _barycentric = barycentric;
}
