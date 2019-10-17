#include "blue/gpu/handlers/CreateFramebufferHandler.hpp"
#include "blue/Context.hpp"

namespace {

    bool map_attachment(FramebufferAttachmentType type, GLenum &glType) {
        switch (type) {
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

    Framebuffer create_framebuffer(const CreateFramebufferEntity &entity) {
        FramebufferId framebuffer_id = 0;
        TextureId texture_id = 0;

        blue::Context::logger().info("Quering for max framebuffer dimensions.");
        GLint dims[2];
        glGetIntegerv(GL_MAX_VIEWPORT_DIMS, &dims[0]);
        blue::Context::logger().info("Max dimensions: {} {}", dims[0], dims[1]);

        DebugGlCall(glGenFramebuffers(1, &framebuffer_id));
        DebugGlCall(glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_id));

        if (entity.with_texture) {

            DebugGlCall(glGenTextures(1, &texture_id));
            DebugGlCall(glBindTexture(GL_TEXTURE_2D, texture_id));

            // TODO: This needs a generalized case, not only GL_DEPTH_COMPONENT; add format as parameter to entity;
            // How about handling this by spawning another entity and synchronizing via future/promise?
            // Or, already initialized texture ID is passed instead of "with_texture".
            DebugGlCall(
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, entity.texture_width, entity.texture_height, 0,
                                 GL_DEPTH_COMPONENT, GL_FLOAT, NULL));

            DebugGlCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
            DebugGlCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));

            DebugGlCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER));
            DebugGlCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER));

            GLfloat borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
            DebugGlCall(glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor));

            // Attach texture to framebuffer

            DebugGlCall(glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_id));

            GLenum attachmentType;
            BLUE_ASSERT(map_attachment(entity.attachmentType, attachmentType));

            DebugGlCall(glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, GL_TEXTURE_2D, texture_id, 0));

            if (entity.attachmentType == FramebufferAttachmentType::DEPTH_ATTACHMENT) {
                // No color buffers are written.
                DebugGlCall(glDrawBuffer(GL_NONE));
                DebugGlCall(glReadBuffer(GL_NONE));
            }

            DebugGlCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));

            if (texture_id) {
                blue::Context::logger().info("Created framebuffer's texture attachment successfuly ({}).", texture_id);
            } else {
                blue::Context::logger().error("Failed to create framebuffer's texture attachment.");
            }
        }

        auto framebufferStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (framebuffer_id &&  framebufferStatus == GL_FRAMEBUFFER_COMPLETE) {
            blue::Context::logger().info("Created framebuffer successfuly ({}).", framebuffer_id);
        } else {
            blue::Context::logger().error("Failed to create framebuffer. Framebuffer status: {}", framebufferStatus);
        }

        // Unbiding. FIXME: This may affect render pipeline.
        DebugGlCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));

        Framebuffer framebuffer;
        framebuffer.framebuffer = framebuffer_id;
        framebuffer.texture = texture_id;
        framebuffer.texture_width = entity.texture_width;
        framebuffer.texture_height = entity.texture_height;
        return framebuffer;
    }
}

void handle(std::pair<std::promise<Framebuffer>, CreateFramebufferEntity> &pair) {
    std::promise<Framebuffer> &promise = pair.first;
    const CreateFramebufferEntity &entity = pair.second;
    promise.set_value(create_framebuffer(entity));
}
