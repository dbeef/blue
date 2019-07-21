#pragma once

#ifndef NDEBUG

#include "glad/glad.h"
#include <iostream>

static void GlClearError() {
	while (glGetError() != GL_NO_ERROR);
}

static bool GlLogCall(const char* function, const char* file, int line) {
	while (GLenum error = glGetError())
	{
		std::cout << "OpenGL error: " << error << " in function: " << function <<
			", in file: " << file << ", line: " << line << std::endl;
		return false;
	}
	return true;
}

#if defined BLUE_WINDOWS
#	define ASSERT(x) if(!(x)) __debugbreak();
#else
#	define ASSERT(x) if(!(x)) std::cin.ignore();
#endif

#define DebugGlCall(x) \
	GlClearError(); \
	x;\
	ASSERT(GlLogCall(#x, __FILE__, __LINE__))

// '#' turns into a string,
// __FILE__ unlike __debugbreak is not intrinsic (compiler specific)
// same for __LINE__

#else

#define DebugGlCall(x) \
x;

#endif
