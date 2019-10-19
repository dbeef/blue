#include "blue/gpu/handlers/ReadFramebufferHandler.hpp"
#include "blue/Context.hpp"

namespace
{
	std::uint8_t getNumberOfComponents(TextureReadDataFormat format)
	{
		switch (format)
		{
		case TextureReadDataFormat::RGB: return 3;
		case TextureReadDataFormat::RGBA: return 4;
		case TextureReadDataFormat::ALPHA: return 1;
		case TextureReadDataFormat::COLOR_INDEX: return 1;
		default: BLUE_ASSERT(false); return 0;
		}
	}

	std::uint8_t getComponentSizeBytes(TextureReadDataComponentSize componentSize)
	{
		switch (componentSize)
		{
		case TextureReadDataComponentSize::FLOAT: return 4;
		case TextureReadDataComponentSize::INT: return 4;
		case TextureReadDataComponentSize::UNSIGNED_INT: return 4;
		case TextureReadDataComponentSize::UNSIGNED_BYTE: return 1;
		case TextureReadDataComponentSize::BYTE: return 1;
		case TextureReadDataComponentSize::SHORT: return 2;
		case TextureReadDataComponentSize::UNSIGNED_SHORT: return 2;
		default: BLUE_ASSERT(false); return 0;
		}
	}

	std::vector<char> read_framebuffer(const ReadFramebufferEntity& entity)
	{
		std::vector<char> out;
		const auto& texture = entity.framebuffer.texture;
		std::size_t textureSizeBytes = texture.width * texture.height * getComponentSizeBytes(entity.readComponentSize) * getNumberOfComponents(entity.readFormat);
		out.resize(textureSizeBytes);

		blue::Context::logger().info("Texture size for reading: {}", textureSizeBytes);

		DebugGlCall(glActiveTexture(GL_TEXTURE0 + entity.framebuffer.texture.slot));
		DebugGlCall(glBindTexture(GL_TEXTURE_2D, entity.framebuffer.texture.id));
		DebugGlCall(glBindFramebuffer(GL_FRAMEBUFFER, entity.framebuffer.framebuffer));
		DebugGlCall(glReadPixels(0, 0, texture.width, texture.height, static_cast<GLenum>(entity.readFormat), static_cast<GLenum>(entity.readComponentSize), out.data()));

		return out;
	}
}

void handle(std::pair<std::promise<std::vector<char>>, ReadFramebufferEntity>& pair)
{
	std::promise<std::vector<char>>& promise = pair.first;
	const ReadFramebufferEntity& entity = pair.second;
	promise.set_value(read_framebuffer(entity));
}
