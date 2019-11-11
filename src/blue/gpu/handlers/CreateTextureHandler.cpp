#include "blue/gpu/handlers/CreateTextureHandler.hpp"
#include "blue/Context.hpp"

#include "stb/image.h"

namespace
{
    Texture create(const CreateTextureEntity &entity)
    {
        Texture texture{};

        stbi_uc *buffer = nullptr;

        if (!entity.data->empty())
        {
            int width;
            int height;
            int bits_per_pixel;
            int desiredChannels = 4;

            buffer = stbi_load_from_memory(
                    reinterpret_cast<const stbi_uc *>(entity.data->data()),
                    entity.data->size(),
                    &width,
                    &height,
                    &bits_per_pixel,
                    desiredChannels
            );

            if (buffer == nullptr)
            {
				blue::Context::logger().error("Failed to create texture from passed data. Reason: {}", stbi_failure_reason());
                return texture;
            }

            blue::Context::logger().info("Created texture: {}/{}, {} bpp.", width, height, bits_per_pixel);

            texture.width = static_cast<uint16_t>(width);
            texture.height = static_cast<uint16_t>(height);
            texture.storingFormat = TextureStoringFormat::RGBA8;
            texture.passedDataFormat = TexturePassedDataFormat::RGBA;
            texture.passedDataComponentSize = TexturePassedDataComponentSize::UNSIGNED_BYTE;
        }
        else
        {
            texture.width = entity.width;
            texture.height = entity.height;
            texture.storingFormat = entity.storingFormat;
            texture.passedDataFormat = entity.passedDataFormat;
            texture.passedDataComponentSize = entity.passedDataComponentSize;
        }

        texture.slot = entity.slot;

        // init texture on GPU
        DebugGlCall(glGenTextures(1, &texture.id));
        // Slots are consecutive numbers so we can just add:
        DebugGlCall(glActiveTexture(GL_TEXTURE0 + entity.slot));
        DebugGlCall(glBindTexture(GL_TEXTURE_2D, texture.id));
        DebugGlCall(glTexImage2D(GL_TEXTURE_2D, 0,
                                 static_cast<GLenum>(texture.storingFormat), texture.width, texture.height, 0,
                                 static_cast<GLenum>(texture.passedDataFormat),
                                 static_cast<GLenum>(texture.passedDataComponentSize), buffer));

        if (entity.withMipMaps)
        {
            DebugGlCall(glGenerateMipmap(GL_TEXTURE_2D));
        }

        DebugGlCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, static_cast<GLenum>(entity.filtering)));
        DebugGlCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, static_cast<GLenum>(entity.filtering)));

        // S/T are X/Y coordinates but for textures:
        DebugGlCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, static_cast<GLenum>(entity.wrapping)));
        DebugGlCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, static_cast<GLenum>(entity.wrapping)));

        auto &renderer = blue::Context::renderer();
        renderer.set_cached_texture(entity.slot, texture.id);

        stbi_image_free(buffer);
        blue::Context::logger().info("Created texture with id: {}", texture.id);

        return texture;
    }
}

void handle(std::pair<std::promise<Texture>, CreateTextureEntity> &pair)
{
    std::promise<Texture> &promise = pair.first;
    const CreateTextureEntity &entity = pair.second;

    Texture texture = create(entity);

    promise.set_value(texture);
}
