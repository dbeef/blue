#include "blue/gpu/handlers/CreateTextureHandler.hpp"
#include "blue/Context.hpp"

#include "stb/image.h"

namespace
{
    Texture create(const CreateTextureEntity &entity)
    {
        Texture texture;
        texture.slot = entity.slot;

        int width;
        int height;
        int bits_per_pixel;

        constexpr int DESIRED_CHANNELS = 4;

        stbi_uc *buffer = stbi_load_from_memory(
                reinterpret_cast<const stbi_uc *>(entity.data->data()),
                entity.data->size(),
                &width,
                &height,
                &bits_per_pixel,
                DESIRED_CHANNELS
        );

        if (buffer == nullptr)
        {
            blue::Context::logger().error("Failed to create texture.s");
            return texture;
        }
        else
        {
            blue::Context::logger().info("Created texture: {}/{}, {} bpp.", width, height, bits_per_pixel);
        }

        // init texture on GPU
        DebugGlCall(glGenTextures(1, &texture.id));
        // Slots are consecutive numbers so we can just add:
        DebugGlCall(glActiveTexture(GL_TEXTURE0 + entity.slot));
        DebugGlCall(glBindTexture(GL_TEXTURE_2D, texture.id));
        DebugGlCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer));
        DebugGlCall(glGenerateMipmap(GL_TEXTURE_2D));
        DebugGlCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
        DebugGlCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
        //s/t are coordinates like x/y but for textures

        // Failed to run on ubuntu because GL_CLAMP, fixed by GL_CLAMP_TO_EDGE.
        // https://www.khronos.org/opengl/wiki/Common_Mistakes
        DebugGlCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
        DebugGlCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));

        // Update renderer cache:
        auto &renderer = blue::Context::renderer();
        auto &textures = renderer.get_cached_textures();
        auto slot = textures.find(entity.slot);
        if (slot != textures.end())
        {
            slot->second = texture.id;
        }
        else
        {
            textures.insert({texture.slot, texture.id});
        }

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
