#include "blue/gpu/handlers/CompileShaderHandler.hpp"
#include "blue/Context.hpp"

namespace
{
	// TODO: Remove explicit uniform location once uniform buffers are used.
	const char* getOpenGLVersion() {
#ifdef BLUE_ANDROID
	    // Explicit uniform locations work from 3.1 ES - setting 3.0 with enabled extension:
	    // https://android-developers.googleblog.com/2015/04/game-performance-explicit-uniform.html
		return "#version 300 es\n #extension GL_ARB_explicit_uniform_location : enable \n";
#else
		return "#version 330 core\n #extension GL_ARB_explicit_uniform_location : enable \n";
#endif
	}

	GLuint link_shaders(GLuint vertexShaderId, GLuint fragmentShaderId)
	{
		GLuint shaderProgramId = glCreateProgram();

		glAttachShader(shaderProgramId, vertexShaderId);
		glAttachShader(shaderProgramId, fragmentShaderId);
		glLinkProgram(shaderProgramId);

		GLint success;
		GLchar infoLog[512];
		glGetProgramiv(shaderProgramId, GL_LINK_STATUS, &success);

		if (!success)
		{
			glGetProgramInfoLog(shaderProgramId, 512, NULL, infoLog);
			blue::Context::logger().info("Linking failed for shaders: {} {}", vertexShaderId, fragmentShaderId);
			blue::Context::logger().info("Reason: {}", infoLog);
			return 0;
		}

		glDeleteShader(vertexShaderId);
		glDeleteShader(fragmentShaderId);

		return shaderProgramId;
	}

	GLuint compile_shader(const std::string& source, GLenum shader_type)
	{
		GLuint shaderId = 0;

		std::string source_copy = source;
		source_copy.insert(0, getOpenGLVersion());

		const char* cstr = source_copy.c_str();
		const GLchar* const* c = &cstr;

		shaderId = glCreateShader(shader_type);
		glShaderSource(shaderId, 1, c, NULL);
		glCompileShader(shaderId);

		GLint success;
		GLchar infoLog[512];
		glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);

		if (!success)
		{
			glGetShaderInfoLog(shaderId, 512, NULL, infoLog);
			blue::Context::logger().info("Shader failed to compile ({}).", shaderId);
			blue::Context::logger().info("Reason: {}", infoLog);
			return 0;
		}

		blue::Context::logger().info("Shader compiled successfuly ({}).", shaderId);
		return shaderId;
	}
}

void bind_with_uniform_buffer(ShaderId id)
{
	// TODO: Rename "Matrices" to "Blue_Environment"
	int uniformBlockIndex = glGetUniformBlockIndex(id, "Matrices");
	if (uniformBlockIndex >= 0)
	{
		blue::Context::logger().info("Found blue uniform buffer in shader, binding.");
        DebugGlCall(glUniformBlockBinding(id, uniformBlockIndex, 0));
	}
	else
	{
		blue::Context::logger().warn("Not found blue uniform buffer in shader.");
	}
}

void handle(std::pair<std::promise<ShaderId>, CompileShaderEntity>& pair)
{
	std::promise<ShaderId>& promise = pair.first;
	const CompileShaderEntity& entity = pair.second;

	const auto vertexProgram = compile_shader(entity.vertex, GL_VERTEX_SHADER);
	const auto fragmentProgram = compile_shader(entity.fragment, GL_FRAGMENT_SHADER);
	const auto linked = link_shaders(vertexProgram, fragmentProgram);

	bind_with_uniform_buffer(linked);

	promise.set_value(linked);
}
