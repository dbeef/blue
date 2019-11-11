#include "blue/TextureUtils.hpp"
#include "blue/ResourcesPath.h"
#include "blue/Context.hpp"
#include "stb/image.h"

#include <SDL2/SDL.h>

CreateTextureEntity ImageUtils::read(const std::string& filepath)
{
	// FIXME: No need for flipping when rendering orthographically, but needed when using perspective projection.
	stbi_set_flip_vertically_on_load(false);

	auto data = std::make_shared<std::vector<char>>();
	auto absolute_path = paths::getResourcesPath() + filepath;

	int size;
	SDL_RWops* file = SDL_RWFromFile(absolute_path.c_str(), "rb");

	// TODO: Externalize filesystem operations.
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

	// FIXME: Return data instead
    CreateTextureEntity e;
    e.data = data;
    return e;
}
