layout(location = 0) in lowp vec3 position;
layout(location = 1) in lowp vec2 texCoord;
layout(location = 2) in lowp float layer;
layout(location = 3) in lowp vec3 normal;

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

out lowp vec2 TexCoord;
flat out lowp float Layer;

out vec3 Normal;
out vec3 FragPos;

void main()
{
	gl_Position = projection * view * model * vec4(position, 1.0f);
	TexCoord = vec2(texCoord.x, 1.0 - texCoord.y);
	// Forwarding values to fragment shader
	Layer = layer;
	Normal = normal;
	FragPos = vec3(model * vec4(position, 1.0f));
}
