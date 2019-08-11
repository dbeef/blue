#include "blue/gpu/handlers/CreateFramebufferHandler.hpp"
#include "blue/Context.hpp"

namespace
{
	Framebuffer create_framebuffer(const CreateFramebufferEntity& entity)
	{
		FramebufferId framebuffer_id = 0;
		TextureId texture_id = 0;

		blue::Context::logger().info("Quering for max framebuffer dimensions.");
		GLint dims[2];
		glGetIntegerv(GL_MAX_VIEWPORT_DIMS, &dims[0]);
		blue::Context::logger().info("Max dimensions: {} {}", dims[0], dims[1]);

		DebugGlCall(glGenFramebuffers(1, &framebuffer_id));
		DebugGlCall(glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_id));

		if(entity.with_texture)
		{
			DebugGlCall(glGenTextures(1, &texture_id));
			DebugGlCall(glBindTexture(GL_TEXTURE_2D, texture_id));
			// TODO: This needs a generalized case, not only GL_DEPTH_COMPONENT; add format as parameter to entity.
			DebugGlCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, entity.texture_width, entity.texture_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL));

			DebugGlCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
			DebugGlCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));

			DebugGlCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER));
			DebugGlCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER));

			GLfloat borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
			DebugGlCall(glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor));

			if (texture_id)
			{
				blue::Context::logger().info("Created framebuffer's texture attachment successfuly ({}).", texture_id);
			}
			else
			{
				blue::Context::logger().error("Failed to create framebuffer's texture attachment.");
			}
		}

		DebugGlCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));

		if (framebuffer_id)
		{
			blue::Context::logger().info("Created framebuffer successfuly ({}).", framebuffer_id);
		}
		else
		{
			blue::Context::logger().error("Failed to create framebuffer.");
		}

		Framebuffer framebuffer;
		framebuffer.framebuffer = framebuffer_id;
		framebuffer.texture = texture_id;
		framebuffer.texture_width = entity.texture_width;
		framebuffer.texture_height = entity.texture_height;
		return framebuffer;
	}
}

void handle(std::pair<std::promise<Framebuffer>, CreateFramebufferEntity>& pair)
{
	std::promise<Framebuffer
	>& promise = pair.first;
	const CreateFramebufferEntity& entity = pair.second;
	promise.set_value(create_framebuffer(entity));
}
