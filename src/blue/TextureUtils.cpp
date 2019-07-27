#include "blue/TextureUtils.hpp"
#include "blue/ResourcesPath.h"
#include "blue/Context.hpp"
#include "stb/image.h"

#include <SDL2/SDL.h>
#include <vector>
#include <memory>

CreateTextureEntity ImageUtils::read(const std::string& filepath)
{
	stbi_set_flip_vertically_on_load(true);

	auto data = std::make_shared<std::vector<char>>();
	auto absolute_path = paths::getResourcesPath() + filepath;

	int size;
	SDL_RWops* file = SDL_RWFromFile(absolute_path.c_str(), "rb");

	if (file != nullptr) 
	{
		size = file->size(file);
		//Initialize data
		data->resize(size);
		file->read(file, &(*data)[0], size, 1);
		//Close file handler
		SDL_RWclose(file);
		blue::Context::logger().info("Succesfuly read image data.");
	}
	else 
	{
		blue::Context::logger().error("Unable to open file, SDL error: {}", SDL_GetError());
	}

	return { data };
}
