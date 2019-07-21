#include "blue/gpu/handlers/CreateMeshHandler.hpp"
#include "blue/Context.hpp"

void handle(SetClearColorEntity& entity)
{
	DebugGlCall(glClearColor(entity.color[0], entity.color[1], entity.color[2], 1.0f));
}
