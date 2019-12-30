#include "blue/gpu/handlers/UpdateEnvironmentHandler.hpp"
#include "blue/Context.hpp"

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>

// Uniform buffer layout described in CreateUboHandler.cpp

namespace
{
	inline void upload_chunk(UniformBufferId id, const void* data, int offset, int size)
	{
        DebugGlCall(glBindBuffer(GL_UNIFORM_BUFFER, id));
		DebugGlCall(glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data));
		blue::Context::renderer().invalidate_cache_uniform_buffer();
	}
}

void handle(UpdateEnvironmentEntity_CameraPos& entity)
{
	upload_chunk(entity.id, glm::value_ptr(entity.camera_position), 176, 12);
}

void handle(UpdateEnvironmentEntity_LightPos& entity)
{
	upload_chunk(entity.id, glm::value_ptr(entity.light_position), 160, 12);
}

void handle(UpdateEnvironmentEntity_Projection& entity)
{
	upload_chunk(entity.id, glm::value_ptr(entity.projection), 64, 64);
}

void handle(UpdateEnvironmentEntity_LightSpaceMatrix& entity)
{
	upload_chunk(entity.id, glm::value_ptr(entity.light_space_matrix), 192, 64);
}

void handle(UpdateEnvironmentEntity_View& entity)
{
	upload_chunk(entity.id, glm::value_ptr(entity.view), 0, 64);
}

void handle(UpdateEnvironmentEntity_AmbientStrength& entity)
{
	upload_chunk(entity.id, &entity.strength, 128, 4);
}

void handle(UpdateEnvironmentEntity_LightColor& entity)
{
	upload_chunk(entity.id, glm::value_ptr(entity.color), 144, 12);
}
