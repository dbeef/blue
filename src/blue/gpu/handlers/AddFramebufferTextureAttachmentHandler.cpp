#include <blue/Context.hpp>
#include "blue/gpu/handlers/AddFramebufferTextureAttachmentHandler.hpp"

bool map_attachment(FramebufferAttachmentType type, GLenum &glType)
{
    switch (type)
    {
        case FramebufferAttachmentType::DEPTH_ATTACHMENT:
            glType = GL_DEPTH_ATTACHMENT;
            return true;
        case FramebufferAttachmentType::STENCIL_ATTACHMENT:
            glType = GL_STENCIL_ATTACHMENT;
            return true;
        case FramebufferAttachmentType::DEPTH_STENCIL_ATTACHMENT:
            glType = GL_DEPTH_STENCIL_ATTACHMENT;
            return true;
            // Framebuffers can have multiple color attachments; for now defaulting to color attachment 0,
            // as it is most common case.
        case FramebufferAttachmentType::COLOR_ATTACHMENT:
            glType = GL_COLOR_ATTACHMENT0;
            return true;
        default:
            return false;
    }
}

void handle(std::pair<std::promise<bool>, AddFramebufferTextureAttachmentEntity> &pair)
{
    std::promise<bool> &promise = pair.first;
    const AddFramebufferTextureAttachmentEntity &entity = pair.second;

    DebugGlCall(glActiveTexture(GL_TEXTURE0 + entity.framebuffer.texture.slot));
    DebugGlCall(glBindTexture(GL_TEXTURE_2D, entity.framebuffer.texture.id));
    DebugGlCall(glBindFramebuffer(GL_FRAMEBUFFER, entity.framebuffer.framebuffer));

    // Update renderer cache:
    auto &renderer = blue::Context::renderer();
    renderer.set_cached_framebuffer(entity.framebuffer.framebuffer);
    renderer.set_cached_texture(entity.framebuffer.texture.slot, entity.framebuffer.texture.id);
    // Part of updating cache:
    DebugGlCall(glViewport(0, 0, entity.framebuffer.texture.width, entity.framebuffer.texture.height));

    GLenum attachmentType;
    BLUE_ASSERT(map_attachment(entity.attachmentType, attachmentType));
    DebugGlCall(
            glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, GL_TEXTURE_2D, entity.framebuffer.texture.id, 0));

    if (entity.attachmentType == FramebufferAttachmentType::DEPTH_ATTACHMENT)
    {
#if !defined BLUE_ANDROID
        // No color buffers are written.
        DebugGlCall(glDrawBuffer(GL_NONE));
        DebugGlCall(glReadBuffer(GL_NONE));
#endif
    }
    else
    {
        DebugGlCall(glDrawBuffer(GL_COLOR_ATTACHMENT0));
        DebugGlCall(glReadBuffer(GL_COLOR_ATTACHMENT0));
    }

    auto framebufferStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (framebufferStatus == GL_FRAMEBUFFER_COMPLETE)
    {
        blue::Context::logger().info("Added framebuffer texture attachment successfuly");
        promise.set_value(true);
    }
    else
    {
        blue::Context::logger().error("Failed to add texture attachment. Framebuffer status: {}", framebufferStatus);
        promise.set_value(false);
    }
}
