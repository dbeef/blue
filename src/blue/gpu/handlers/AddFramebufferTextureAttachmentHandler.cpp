#include <blue/Context.hpp>
#include "blue/gpu/handlers/AddFramebufferTextureAttachmentHandler.hpp"

//            // TODO: This needs a generalized case, not only GL_DEPTH_COMPONENT; add format as parameter to entity;
//            // How about handling this by spawning another entity and synchronizing via future/promise?
//            // Or, already initialized texture ID is passed instead of "with_texture".
//            DebugGlCall(
//                    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, entity.texture_width, entity.texture_height, 0,
//                                 GL_DEPTH_COMPONENT, GL_FLOAT, NULL));
//
//            DebugGlCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
//            DebugGlCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
//
//            DebugGlCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER));
//            DebugGlCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER));
//
//            GLfloat borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
//            DebugGlCall(glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor));

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

void handle(AddFramebufferTextureAttachmentEntity &entity)
{
    DebugGlCall(glActiveTexture(GL_TEXTURE0 + entity.framebuffer.texture.slot));
    DebugGlCall(glBindTexture(GL_TEXTURE_2D, entity.framebuffer.texture.id));
    DebugGlCall(glBindFramebuffer(GL_FRAMEBUFFER, entity.framebuffer.framebuffer));

    // Update renderer cache:
    auto &renderer = blue::Context::renderer();
    renderer.set_cached_framebuffer(entity.framebuffer.framebuffer);
    renderer.set_cached_texture(entity.framebuffer.texture.slot, entity.framebuffer.texture.id);

    GLenum attachmentType;
    BLUE_ASSERT(map_attachment(entity.attachmentType, attachmentType));
    DebugGlCall(glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, GL_TEXTURE_2D, entity.framebuffer.texture.id, 0));

    if (entity.attachmentType == FramebufferAttachmentType::DEPTH_ATTACHMENT)
    {
        // No color buffers are written.
        DebugGlCall(glDrawBuffer(GL_NONE));
        DebugGlCall(glReadBuffer(GL_NONE));
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
    }
    else
    {
        blue::Context::logger().error("Failed to add texture attachment. Framebuffer status: {}", framebufferStatus);
    }
}
