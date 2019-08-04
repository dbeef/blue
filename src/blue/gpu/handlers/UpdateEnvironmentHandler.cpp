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
	inline void upload_chunk(const void* data, int offset, int size) 
	{
		DebugGlCall(glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data));
	}
}

void handle(UpdateEnvironmentEntity_CameraPos& entity)
{
	upload_chunk(glm::value_ptr(entity.camera_position), 176, 12);
}

void handle(UpdateEnvironmentEntity_Projection& entity)
{
	upload_chunk(glm::value_ptr(entity.projection), 64, 64);
}

void handle(UpdateEnvironmentEntity_View& entity)
{
	upload_chunk(glm::value_ptr(entity.view), 0, 64);
}
