layout(location = 0) in lowp vec3 position;
layout(location = 1) in lowp vec3 color;

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


flat out lowp vec3 Color;

void main()
{
	gl_Position = projection * view * model * vec4(position, 1.0f);
	Color = color;
}