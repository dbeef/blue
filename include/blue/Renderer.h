#pragma once

#include <glm/gtx/quaternion.hpp>

#include <vector>
#include <atomic>
#include <utility>
#include <functional>

#include "blue/GlDebugging.h"
#include "blue/Assertions.h"
#include "blue/gpu/GpuEntities.hpp"
#include "glad/glad.h"

using ImGuiEntity = std::function<void()>;
using RenderEntityId = std::uint32_t;
using ImGuiEntityId = std::uint32_t;

struct RenderEntity
{
	ShaderId shader{};
	RenderEntityId id{};
	VertexArray vertex_array{};
	glm::vec3 position{};
	glm::quat rotation{};
	float scale{};
	UniformBufferId environment{};
	TextureId texture{};
	//
	GLenum stencil_function = GL_ALWAYS;
	GLuint stencil_mask = 0xFF;
	//
	Framebuffer framebuffer{};
};

// 64 bytes of size - fills CPU cache line on IA32/IA64 and ARM A-series (most of smartphones).
// It is performance optimization, to keep RenderEntity from being too bloated and get boost 
// from prefetching on modern CPU's.
// static_assert(sizeof(RenderEntity) <= 64, "Render entity too big.");

class Renderer {

public:

	friend class GpuThread;

	ImGuiEntityId add(const ImGuiEntity& entity);
	RenderEntityId add(const RenderEntity& entity);
	void update(const RenderEntity& entity);
	void remove_render_entity(const RenderEntityId& id);
	void remove_imgui_entity(const ImGuiEntityId& id);
    void invalidate_cache();
    void set_cached_shader(ShaderId program);

private:

    struct
    {
        ShaderId current_shader = 0;
        TextureId current_texture = 0;
        UniformBufferId current_environment = 0;
		FramebufferId current_framebuffer = 0;
    } cache;

	void draw_render_entities();
	void draw_imgui_entities();
	void clear() const;

	void lock();
	void unlock();

	void sort_entities_by_shader();

	std::atomic_bool locked{ false };
	std::vector<RenderEntity> render_entities;
	std::vector<std::pair<ImGuiEntityId, ImGuiEntity>> imgui_entities;
};
