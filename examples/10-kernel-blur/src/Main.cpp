#include <blue/Context.hpp>
#include <blue/Timestep.hpp>
#include <blue/ShaderUtils.h>
#include <blue/TextureUtils.hpp>
#include <blue/camera/OrthographicCamera.hpp>
#include <stb/image_write.h>

#include <atomic>

namespace
{
	const int viewport_width = 512;
	const int viewport_height = 512;
}

int main(int argc, char* argv[])
{
	blue::Context::init();
	blue::Context::window().create_hidden();
	blue::Context::gpu_thread().run();

	std::atomic_bool running{ true };
	blue::Context::input().registerKeyCallback({
													   [&running]()
													   { running = false; },
													   SDLK_ESCAPE,
													   SDL_KEYDOWN
		}
	);

	auto compile_shader_entity = ShaderUtils::make_entity(
		"resources/Triangle.vertex.glsl",
		"resources/Triangle.fragment.glsl");
	auto shader = blue::Context::gpu_system().submit(compile_shader_entity).get();

	auto kernel_blur_compile_shader_entity = ShaderUtils::make_entity(
		"resources/KernelBlur.vertex.glsl",
		"resources/KernelBlur.fragment.glsl");
	auto kernel_blur_shader = blue::Context::gpu_system().submit(kernel_blur_compile_shader_entity).get();

	Vertices vertices =
	{
		/* Vertex pos */ -1.0f, -1.0f, /* Tex coord */ 0.0f, 0.0f,
		/* Vertex pos */ -1.0f, 1.0f,  /* Tex coord */ 0.0f, 1.0f,
		/* Vertex pos */ 1.0f, 1.0f,   /* Tex coord */ 1.0f, 1.0f,
		/* Vertex pos */ 1.0f, -1.0f,  /* Tex coord */ 1.0f, 0.0f,
	};

	Indices indices =
	{
			0, 1, 2, 2, 3, 0
	};

	Attributes attributes =
	{
			{ShaderAttribute::Type::VEC2, ShaderAttribute::Purpose::VERTEX_POSITION,    ShaderAttribute::Buffer::VERTEX},
			{ShaderAttribute::Type::VEC2, ShaderAttribute::Purpose::TEXTURE_COORDINATE, ShaderAttribute::Buffer::VERTEX}
	};

	auto vertex_array = blue::Context::gpu_system().submit(CreateMeshEntity{ vertices, indices, attributes, 6 }).get();

	OrthographicCamera camera(OrthographicCamera::Mode::SCREEN_SPACE, viewport_width, viewport_height);
	auto environment = blue::Context::gpu_system().submit(CreateEnvironmentEntity{}).get();
	blue::Context::gpu_system().submit(UpdateEnvironmentEntity_Projection{ environment, camera.get_projection() });
	blue::Context::gpu_system().submit(UpdateEnvironmentEntity_View{ environment, camera.get_view() });

	auto create_texture_entity = ImageUtils::read("resources/blue.png");
	create_texture_entity.slot = 3;

	auto texture = blue::Context::gpu_system().submit(create_texture_entity).get();

	blue::Context::gpu_system().submit(UpdateUniformVariableEntity{ ShaderAttribute::Type::INT, &create_texture_entity.slot, shader, 9, "" });
	blue::Context::gpu_system().submit(UpdateUniformVariableEntity{ ShaderAttribute::Type::INT, &create_texture_entity.slot, kernel_blur_shader, 9, "" });

	auto framebuffer = blue::Context::gpu_system().submit(CreateFramebufferEntity{}).get();
	auto renderTexture = blue::Context::gpu_system().submit(CreateTextureEntity{
			std::make_shared<std::vector<char>>(), true,
			TextureFiltering::LINEAR,
			TextureWrapping::CLAMP_TO_EDGE,
			viewport_width,
			viewport_height,
			3,
			TexturePassedDataFormat::RGB,
			TextureStoringFormat::RGB8,
			TexturePassedDataComponentSize::UNSIGNED_BYTE
		}).get();
	framebuffer.texture = renderTexture;
	blue::Context::gpu_system().submit(AddFramebufferTextureAttachmentEntity{ framebuffer, FramebufferAttachmentType::COLOR_ATTACHMENT }).wait();

	RenderEntity entity;
	entity.position = { viewport_width / 2, viewport_height / 2, 0.0f };
	entity.shader = shader;
	entity.vertex_array = vertex_array;
	entity.scale = 256.0f; // FIXME: Scale x, scale y, scale z? 
	entity.rotation = glm::identity<glm::quat>();
	entity.environment = environment;
	entity.textures[0] = texture; // FIXME: Array of textures?
	entity.framebuffer = framebuffer;

	RenderEntityId id = blue::Context::renderer().add(entity);
	blue::Context::gpu_thread().wait_for_render_pass();

	auto secondPassTexture = blue::Context::gpu_system().submit(CreateTextureEntity{
	std::make_shared<std::vector<char>>(), true,
	TextureFiltering::LINEAR,
	TextureWrapping::CLAMP_TO_EDGE,
	viewport_width,
	viewport_height,
	3,
	TexturePassedDataFormat::RGB,
	TextureStoringFormat::RGB8,
	TexturePassedDataComponentSize::UNSIGNED_BYTE
		}).get();
	framebuffer.texture = secondPassTexture;
	blue::Context::gpu_system().submit(AddFramebufferTextureAttachmentEntity{ framebuffer, FramebufferAttachmentType::COLOR_ATTACHMENT }).wait();

	blue::Context::renderer().remove_render_entity(id);
	blue::Context::gpu_thread().wait_for_render_pass();

	RenderEntity secondPassEntity;
	secondPassEntity.position = { viewport_width / 2, viewport_height / 2, 0.0f };
	secondPassEntity.shader = kernel_blur_shader;
	secondPassEntity.vertex_array = vertex_array;
	secondPassEntity.scale = 256.0f;
	secondPassEntity.rotation = glm::identity<glm::quat>();
	secondPassEntity.environment = environment;
	secondPassEntity.textures[0] = renderTexture;
	secondPassEntity.framebuffer = framebuffer;

	RenderEntityId secondPassEntityId = blue::Context::renderer().add(secondPassEntity);

	blue::Context::gpu_thread().wait_for_render_pass();

	ReadFramebufferEntity readFramebufferEntity{ framebuffer, TextureReadDataFormat::RGB, TextureReadDataComponentSize::UNSIGNED_BYTE };
	auto framebufferData = blue::Context::gpu_system().submit(readFramebufferEntity).get();

	stbi_flip_vertically_on_write(false);
	stbi_write_bmp("dupsko.bmp", viewport_width, viewport_height, 3, framebufferData.data());

	blue::Context::gpu_thread().stop();
	blue::Context::dispose();

	return 0;
}
