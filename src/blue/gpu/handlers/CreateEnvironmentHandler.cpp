#include <blue/gpu/handlers/CreateEnvironmentHandler.hpp>
#include "blue/Context.hpp"

// "Blue" uniform buffer layout
//
//    layout (std140) uniform Matrices
//            {                              // ALIGNED OFFSET  BASE ALIGNMENT    SIZE
//                    mat4 view;             // 0                 16               64
//                    mat4 projection;       // 64                16               64
//                    float ambientStrength; // 128                4                4
//                    vec3 lightColor;       // 144               16               12
//                    vec3 lightPos;         // 160               16               12
//                    vec3 cameraPos;        // 176               16               12
//                    mat4 lightSpaceMatrix; // 192               16               64
//                                           // TOTAL BUFFER SIZE --------------  256
//            };
//

void handle(std::pair<std::promise<UniformBufferId>, CreateEnvironmentEntity>& pair)
{
	std::promise<UniformBufferId>& promise = pair.first;
	const CreateEnvironmentEntity& entity = pair.second;

	const int buffer_size = 256;

	GLuint id;

	DebugGlCall(glGenBuffers(1, &id));
	DebugGlCall(glBindBuffer(GL_UNIFORM_BUFFER, id));
	DebugGlCall(glBufferData(GL_UNIFORM_BUFFER, buffer_size, nullptr, GL_STATIC_DRAW));
	DebugGlCall(glBindBufferRange(GL_UNIFORM_BUFFER, 0, id, 0, buffer_size));

	// TODO: Invalidate only uniform buffer, probably move invalidate_cache call higher
	blue::Context::renderer().invalidate_cache_uniform_buffer();

	if (id == 0)
	{
		blue::Context::logger().error("Failed to create uniform buffer object.");
	}
	else
	{
		blue::Context::logger().info("Created uniform buffer object successfuly ({}).", id);
	}

	promise.set_value(id);
}
