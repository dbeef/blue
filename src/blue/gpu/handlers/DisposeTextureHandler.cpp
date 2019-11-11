#include "blue/gpu/handlers/CreateTextureHandler.hpp"
#include "blue/Context.hpp"

#include "stb/image.h"

namespace
{
    void dispose(const DisposeTextureEntity &entity)
    {
		blue::Context::logger().info("Disposing texture with id: {}", entity.texture.id);
		DebugGlCall(glDeleteTextures(1, &entity.texture.id));
    }
}

void handle(DisposeTextureEntity& entity)
{
	dispose(entity);
}
