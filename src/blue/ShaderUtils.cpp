#include "blue/ShaderUtils.h"
#include "blue/ResourcesPath.h"
#include "blue/Context.hpp"

#include <SDL2/SDL_rwops.h>

namespace ShaderUtils
{
	std::string read(const std::string& filename) {

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

	CompileShaderEntity make_entity(const std::string vertex_path, const std::string fragment_path)
	{
		CompileShaderEntity entity;
		entity.vertex = ShaderUtils::read(vertex_path);
		entity.fragment = ShaderUtils::read(fragment_path);
		return entity;
	}
}
