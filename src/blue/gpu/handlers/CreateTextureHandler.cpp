#include "blue/gpu/handlers/CreateTextureHandler.hpp"
#include "blue/Context.hpp"

#include "stb/image.h"

namespace
{
	TextureId create(const CreateTextureEntity& entity)
	{
		int width;
		int height;
		int size;
		int bits_per_pixel;

		constexpr int DESIRED_CHANNELS = 4;

		stbi_uc* buffer = stbi_load_from_memory(
			reinterpret_cast<const stbi_uc*>(entity.data->data()),
			entity.data->size(),
			&width,
			&height,
			&bits_per_pixel,
			DESIRED_CHANNELS
		);

		if (buffer == nullptr)
		{
			blue::Context::logger().error("Failed to create texture.s");
			return 0;
		}
		else
		{
			blue::Context::logger().info("Created texture: {}/{}, {} bpp.", width, height, bits_per_pixel);
		}

		TextureId texture = 0;

		// init texture on GPU
		DebugGlCall(glGenTextures(1, &texture));
		// Slots are consecutive numbers so we can just add:
		DebugGlCall(glActiveTexture(GL_TEXTURE0 + 0));
		DebugGlCall(glBindTexture(GL_TEXTURE_2D, texture));
		DebugGlCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer));
		DebugGlCall(glGenerateMipmap(GL_TEXTURE_2D));
		DebugGlCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
		DebugGlCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
		//s/t are coordinates like x/y but for textures

		// Failed to run on ubuntu because GL_CLAMP, fixed by GL_CLAMP_TO_EDGE.
		// https://www.khronos.org/opengl/wiki/Common_Mistakes
		DebugGlCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
		DebugGlCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));

		// send data from RAM to video RAM
		DebugGlCall(glBindTexture(GL_TEXTURE_2D, 0));

		stbi_image_free(buffer);

		blue::Context::logger().info("Created texture with id: {}", texture);

		return texture;
	}
}

void handle(std::pair<std::promise<TextureId>, CreateTextureEntity>& pair)
{
	std::promise<ShaderId>& promise = pair.first;
	const CreateTextureEntity& entity = pair.second;

	TextureId texture = create(entity);

	promise.set_value(texture);
}
