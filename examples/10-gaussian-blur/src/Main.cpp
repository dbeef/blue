#include <blue/Context.hpp>
#include <blue/Timestep.hpp>
#include <blue/ShaderUtils.h>
#include <blue/TextureUtils.hpp>
#include <blue/camera/OrthographicCamera.hpp>
#include <stb/image_write.h>

#include <atomic>

int main(int argc, char* argv[])
{
	blue::Context::init();
	blue::Context::window().create(512, 512);
	blue::Context::gpu_thread().run();

	// Register ESC key callback:
	std::atomic_bool running{ true };

	blue::Context::input().registerKeyCallback({
													   [&running]()
													   { running = false; },
													   SDLK_ESCAPE,
													   SDL_KEYDOWN
		}
	);

	// Now render thread is running and waiting for commands to process,
	// leaving this thread only for CPU logics.

	// Issue the GPU thread with task of compiling shader program:

	auto compile_shader_entity = ShaderUtils::make_entity("resources/Triangle.vertex.glsl",
		"resources/Triangle.fragment.glsl");
	auto shader = blue::Context::gpu_system().submit(compile_shader_entity).get();
	
	auto kernel_blur_compile_shader_entity = ShaderUtils::make_entity("resources/KernelBlur.vertex.glsl",
		"resources/KernelBlur.fragment.glsl");
	auto kernel_blur_shader = blue::Context::gpu_system().submit(kernel_blur_compile_shader_entity).get();

	// Issue the GPU thread with task of uploading mesh:

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

	// Create environment

	auto environment = blue::Context::gpu_system().submit(CreateEnvironmentEntity{}).get();

	// Upload camera's matrices

	OrthographicCamera camera(OrthographicCamera::Mode::SCREEN_SPACE);

	// Create texture
	auto create_texture_entity = ImageUtils::read("resources/blue.png");
	create_texture_entity.slot = 3; // Arbitrary value, just to show that slots can be changed.

	auto texture_future = blue::Context::gpu_system().submit(create_texture_entity);
	texture_future.wait();
	auto texture = texture_future.get();

	blue::Context::gpu_system().submit(UpdateEnvironmentEntity_Projection{ environment, camera.get_projection() });
	blue::Context::gpu_system().submit(UpdateEnvironmentEntity_View{ environment, camera.get_view() });

	auto framebuffer = blue::Context::gpu_system().submit(CreateFramebufferEntity{}).get();
	auto depthTexture = blue::Context::gpu_system().submit(CreateTextureEntity{
			std::make_shared<std::vector<char>>(), true,
			TextureFiltering::LINEAR,
			TextureWrapping::CLAMP_TO_EDGE,
			blue::Context::window().get_width(),
			blue::Context::window().get_height(),
			3,
			TexturePassedDataFormat::RGB,
			TextureStoringFormat::RGB8,
			TexturePassedDataComponentSize::UNSIGNED_BYTE
		}).get();
	framebuffer.texture = depthTexture;
	blue::Context::gpu_system().submit(AddFramebufferTextureAttachmentEntity{ framebuffer, FramebufferAttachmentType::COLOR_ATTACHMENT });

	// Now set used texture slot in shader:
	blue::Context::gpu_system().submit(
		UpdateUniformVariableEntity{ ShaderAttribute::Type::INT, &create_texture_entity.slot, shader, 9, "" });
	
	// Now set used texture slot in shader:
	blue::Context::gpu_system().submit(
		UpdateUniformVariableEntity{ ShaderAttribute::Type::INT, &create_texture_entity.slot, kernel_blur_shader, 9, "" });

	// Submit render command consisting of compiled shader, uploaded mesh and following geometry properties:

	RenderEntity entity;
	entity.position = { blue::Context::window().get_width()/ 2, blue::Context::window().get_height() / 2, 0.0f };
	entity.shader = shader;
	entity.vertex_array = vertex_array;
	entity.scale = 256.0f;
	entity.rotation = glm::identity<glm::quat>();
	entity.environment = environment;
	entity.texture1 = texture;
	entity.framebuffer = framebuffer;

	// FIXME: Need to synchronize with adding texture attachment to framebuffer.
	using namespace std::chrono_literals;
	std::this_thread::sleep_for(250ms);

	RenderEntityId id = blue::Context::renderer().add(entity);

	using namespace std::chrono_literals;
	std::this_thread::sleep_for(250ms);

	auto secondPassTexture = blue::Context::gpu_system().submit(CreateTextureEntity{
	std::make_shared<std::vector<char>>(), true,
	TextureFiltering::LINEAR,
	TextureWrapping::CLAMP_TO_EDGE,
	blue::Context::window().get_width(),
	blue::Context::window().get_height(),
	3,
	TexturePassedDataFormat::RGB,
	TextureStoringFormat::RGB8,
	TexturePassedDataComponentSize::UNSIGNED_BYTE
		}).get();
	framebuffer.texture = secondPassTexture;
	blue::Context::gpu_system().submit(AddFramebufferTextureAttachmentEntity{ framebuffer, FramebufferAttachmentType::COLOR_ATTACHMENT });

	using namespace std::chrono_literals;
	std::this_thread::sleep_for(250ms);

	blue::Context::renderer().remove_render_entity(id);

	using namespace std::chrono_literals;
	std::this_thread::sleep_for(250ms);

	RenderEntity secondPassEntity;
	secondPassEntity.position = { blue::Context::window().get_width() / 2, blue::Context::window().get_height() / 2, 0.0f };
	secondPassEntity.shader = kernel_blur_shader;
	secondPassEntity.vertex_array = vertex_array;
	secondPassEntity.scale = 256.0f;
	secondPassEntity.rotation = glm::identity<glm::quat>();
	secondPassEntity.environment = environment;
	secondPassEntity.texture1 = depthTexture;
	secondPassEntity.framebuffer = framebuffer;

	RenderEntityId secondPassEntityId = blue::Context::renderer().add(secondPassEntity);

	using namespace std::chrono_literals;
	std::this_thread::sleep_for(250ms);

	ReadFramebufferEntity readFramebufferEntity{ framebuffer, TextureReadDataFormat::RGB, TextureReadDataComponentSize::UNSIGNED_BYTE};
	auto framebufferData = blue::Context::gpu_system().submit(readFramebufferEntity).get();

	//stbi_flip_vertically_on_write(true); // As glReadPixels will return flipped image.
	stbi_write_bmp("dupsko.bmp", blue::Context::window().get_width(), blue::Context::window().get_height(), 3, framebufferData.data());

	// Start logics loop with timestep limited to 30 times per second:
	Timestep timestep(30);

	while (running)
	{
		timestep.mark_start();
		blue::Context::input().poll();
		timestep.mark_end();
		timestep.delay();
	}

	blue::Context::gpu_thread().stop();
	blue::Context::dispose();

	return 0;
}
