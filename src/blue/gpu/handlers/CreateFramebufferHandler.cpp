#include "blue/gpu/handlers/CreateFramebufferHandler.hpp"
#include "blue/Context.hpp"

namespace
{
    Framebuffer create_framebuffer(const CreateFramebufferEntity &entity)
    {
        FramebufferId framebuffer_id = 0;

        blue::Context::logger().info("Quering for max viewport dimensions.");
        GLint dims[2];
        DebugGlCall(glGetIntegerv(GL_MAX_VIEWPORT_DIMS, &dims[0]));
        blue::Context::logger().info("Max viewport dimensions: {} {}", dims[0], dims[1]);
        DebugGlCall(glGenFramebuffers(1, &framebuffer_id));

        Framebuffer framebuffer;
        framebuffer.framebuffer = framebuffer_id;
        return framebuffer;
    }
}

void handle(std::pair<std::promise<Framebuffer>, CreateFramebufferEntity> &pair)
{
    std::promise<Framebuffer> &promise = pair.first;
    const CreateFramebufferEntity &entity = pair.second;
    promise.set_value(create_framebuffer(entity));
}
