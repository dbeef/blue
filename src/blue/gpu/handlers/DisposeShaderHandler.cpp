#include "blue/gpu/handlers/DisposeShaderHandler.hpp"
#include "blue/Context.hpp"

void handle(const DisposeShaderEntity& entity)
{
	blue::Context::logger().info("Disposing shader program with id: {}", entity.shader);
	DebugGlCall(glDeleteShader(entity.shader));
}
