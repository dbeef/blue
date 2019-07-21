#pragma once

#include <glm/gtx/quaternion.hpp>

#include <vector>
#include <atomic>

#include "blue/GlDebugging.h"
#include "blue/Assertions.h"
#include "blue/gpu/GpuEntities.hpp"
#include "glad/glad.h"

using RenderEntityId = std::uint32_t;

// 64 bytes of size - fills CPU cache line on IA32/IA64 and ARM A-series (most of smartphones).
struct RenderEntity
{
	ShaderId shader;
	RenderEntityId id;
	VertexArray vertex_array; 
	glm::vec3 position; 
	glm::quat rotation; 
	float scale;
	UniformBufferId environment;
};

class Renderer {

public:

	friend class GpuThread;

	RenderEntityId add(const RenderEntity& entity);
	void update(const RenderEntity& entity);
	void remove(const RenderEntityId& id);

private:

	void draw();
	void clear() const;

	void lock();
	void unlock();

	void sort_entities_by_shader();

	std::atomic_bool locked{ false };
	std::vector<RenderEntity> render_entities;
};
