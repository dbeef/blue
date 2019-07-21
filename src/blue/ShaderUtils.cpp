#include "blue/ShaderUtils.h"
#include "blue/ResourcesPath.h"
#include "blue/Context.hpp"

#include <SDL2/SDL_rwops.h>

namespace ShaderUtils
{

	///For loading shaders from filesystem into std::string.
	std::string parseFile(std::string filename) {

		std::string data;

		auto path_extended = paths::getResourcesPath() + filename;

		SDL_RWops* file = SDL_RWFromFile(path_extended.c_str(), "rb");
		if (file != NULL) {
			auto size = file->size(file);
			//Initialize data
			data.resize(size);
			auto read = file->read(file, &data[0], size, 1);
			//Close file handler
			SDL_RWclose(file);
		}
		else
		{
			blue::Context::logger().error("Error: Unable to open shader file. SDL Error: {}", SDL_GetError());
		}

		return data;
	}
}

ShaderSource::ShaderSource(const std::string vertex_path, const std::string fragment_path)
{
	vertex = ShaderUtils::parseFile(vertex_path);
	fragment = ShaderUtils::parseFile(fragment_path);

	if (vertex.size() == 0 || fragment.size() == 0)
	{
		_valid = false;
	}
}
