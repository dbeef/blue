//
// Created by dbeef on 5/26/19.
//

#include "blue/Renderer.h"
#include "blue/Context.hpp"
#include "blue/Timestep.hpp"

#include <memory>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace
{
	// Using explicit uniform locations in GLSL,
	// thus it's guaranteed for them to have these values.
	// See CreateUboHandler.cpp for Uniform Buffer Object values. 
	// TODO: Make sure every shader has explicit uniform locations; inject them to every shader?
	const struct 
	{
		GLint modelLoc = 4;
	} uniform_locations;
}

void Renderer::clear() const
{
	DebugGlCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));
}

void Renderer::draw_imgui_entities()
{
	for (const auto& pair : imgui_entities)
	{
		const auto& imgui_entity = pair.second;
		imgui_entity();
	}
}

void Renderer::draw_render_entities()
{
	static ShaderId current_shader = 0;
	static TextureId current_texture = 0;
	static UniformBufferId current_environment = 0;

	for (const auto& entity : render_entities)
	{
		const auto& vao = entity.vertex_array;

		if (current_shader != entity.shader)
		{
			current_shader = entity.shader;
			DebugGlCall(glUseProgram(current_shader));
		}

		if (current_environment != entity.environment)
		{
			current_environment = entity.environment;
			DebugGlCall(glBindBuffer(GL_UNIFORM_BUFFER, current_environment));
		}

		if (current_texture != entity.texture && entity.texture != 0)
		{
			current_texture = entity.texture;
			DebugGlCall(glBindTexture(GL_TEXTURE_2D, current_texture));
			// Right now, blue utilizes only one texture slot.
			auto loc = glGetUniformLocation(current_shader, "sampler");
			glUniform1i(loc, 0);
		}

		const glm::mat4 RotationMatrix = glm::toMat4(entity.rotation);
		const glm::mat4 ScaleMatrix = glm::scale(glm::identity<glm::mat4>(), glm::vec3(entity.scale));
		const glm::mat4 TranslationMatrix = glm::translate(glm::identity<glm::mat4>(), entity.position);

		const glm::mat4 model = TranslationMatrix * RotationMatrix * ScaleMatrix;
		glUniformMatrix4fv(uniform_locations.modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		if (vao.ibo)
		{
			DebugGlCall(glBindVertexArray(vao.vao));
			DebugGlCall(glDrawElements(GL_TRIANGLES, vao.vertices_count, GL_UNSIGNED_INT, 0));
			DebugGlCall(glBindVertexArray(0));
		}
		else
		{
			// glDrawArrays does not utilize index buffers (it uses consecutive numbers starting from 0 to fill its index buffer),
			// you still need to know what's the size of vertices.
			DebugGlCall(glBindVertexArray(vao.vao));
			DebugGlCall(glDrawArrays(GL_TRIANGLES, 0, vao.vertices_count));
			DebugGlCall(glBindVertexArray(0));
		}
	}
}

ImGuiEntityId Renderer::add(const ImGuiEntity& entity)
{
	lock();
	static ImGuiEntityId id = 1;
	id++;
	imgui_entities.emplace_back(std::make_pair(id, entity));
	unlock();
	return id;
}

RenderEntityId Renderer::add(const RenderEntity& entity)
{
	lock();
	static RenderEntityId id = 1;
	id++;
	render_entities.emplace_back(RenderEntity{ entity.shader, id, entity.vertex_array, entity.position, entity.rotation, entity.scale, entity.environment, entity.texture });
	sort_entities_by_shader();
	unlock();
	return id;
}

void Renderer::update(const RenderEntity& entity)
{
	lock();
	auto iterator = std::find_if(render_entities.begin(), render_entities.end(), [entity](const RenderEntity& list_entity) -> bool {
		return list_entity.id == entity.id;
		});

	if (iterator != render_entities.end())
	{
		iterator->environment = entity.environment;
		iterator->position = entity.position;
		iterator->rotation = entity.rotation;
		iterator->shader = entity.shader;
		iterator->scale = entity.scale;
		iterator->shader = entity.shader;
		iterator->vertex_array = entity.vertex_array;
	}
	unlock();
}

void Renderer::remove_render_entity(const RenderEntityId& id)
{
	lock();
	auto iterator = std::remove_if(render_entities.begin(), render_entities.end(), [id](const RenderEntity& list_entity) -> bool {
		return list_entity.id == id;
		});

	if (iterator != render_entities.end())
	{
		render_entities.erase(iterator);
	}
	unlock();
}

void Renderer::remove_imgui_entity(const ImGuiEntityId& id)
{
	lock();
	auto iterator = std::remove_if(imgui_entities.begin(), imgui_entities.end(), [id](const std::pair<ImGuiEntityId, ImGuiEntity>& list_entity) -> bool {
		return list_entity.first == id;
		});

	if (iterator != imgui_entities.end())
	{
		imgui_entities.erase(iterator);
	}
	unlock();
}

void Renderer::lock()
{
	bool expected = false;
	const bool desired = true;

	while (!locked.compare_exchange_weak(expected, desired))
	{
		expected = false;
	}
}

void Renderer::unlock() {
	locked.store(false);
}

void Renderer::sort_entities_by_shader()
{
	std::sort(render_entities.begin(), render_entities.end(), [](const RenderEntity& first, const RenderEntity& second)
		{
			return first.shader > second.shader;
		});
}
