#include "blue/gpu/handlers/CompileShaderHandler.hpp"
#include "blue/Context.hpp"

void handle(const UpdateUniformVariableEntity& entity)
{
	GLint location = -1;

	if (entity.location)
	{
		location = entity.location;
	}
	else
	{
		DebugGlCall(glGetUniformLocation(entity.program, entity.name.c_str()));
		if (location < 0)
		{
			blue::Context::logger().error("Failed to find location for uniform: {} in shader program: {}", entity.name, entity.program);
			return;
		}
	}

	DebugGlCall(glUseProgram(entity.program));
	blue::Context::renderer().invalidate_cache_shader();

	switch (entity.type)
	{
	case(ShaderAttribute::Type::FLOAT):	
	{
		DebugGlCall(glUniform1f(location, *reinterpret_cast<const GLfloat*>(entity.value)));
		break;
	}
	default: break;
	}
}
