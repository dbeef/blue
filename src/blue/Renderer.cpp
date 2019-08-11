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
#include <blue/Renderer.h>

namespace
{
	// Using explicit uniform locations in GLSL,
	// thus it's guaranteed for them to have these values.
	// See CreateUboHandler.cpp for Uniform Buffer Object values.
	// TODO: Make sure every shader has explicit uniform locations; inject them to every shader?
	const struct
	{
		GLint modelLoc = 0;
	} uniform_locations;
}

void Renderer::clear() const
{
	// TODO: Add an entity + handler for setting this up.
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
	for (const auto& entity : render_entities)
	{
		const auto& vao = entity.vertex_array;
		DebugGlCall(glBindVertexArray(vao.vao));

		if (cache.current_shader != entity.shader)
		{
			cache.current_shader = entity.shader;
			DebugGlCall(glUseProgram(cache.current_shader));
		}

		if (cache.current_environment != entity.environment)
		{
			cache.current_environment = entity.environment;
			DebugGlCall(glBindBuffer(GL_UNIFORM_BUFFER, cache.current_environment));
		}

		if (cache.current_texture != entity.texture && entity.texture != 0)
		{
			cache.current_texture = entity.texture;
			DebugGlCall(glBindTexture(GL_TEXTURE_2D, cache.current_texture));
			// Right now, blue utilizes only one texture slot (GL_TEXTURE0).
			auto loc = glGetUniformLocation(cache.current_shader, "sampler");
			DebugGlCall(glUniform1i(loc, 0));
		}

		if (cache.current_framebuffer != entity.framebuffer.framebuffer)
		{
			cache.current_framebuffer = entity.framebuffer.framebuffer;
			if (cache.current_framebuffer == 0)
			{
				// Finished drawing to framebuffer:
				DebugGlCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
				DebugGlCall(glViewport(0, 0, blue::Context::window().get_width(), blue::Context::window().get_height()));

				DebugGlCall(glStencilFunc(GL_ALWAYS, 1, 0xFF)); // all fragments should update the stencil buffer
				DebugGlCall(glStencilMask(0x00)); // make sure we don't update the stencil buffer, since we finished 

				DebugGlCall(glCullFace(GL_BACK)); // reset to original culling face
				DebugGlCall(glBindTexture(GL_TEXTURE_2D, entity.framebuffer.texture));

				cache.current_texture = entity.framebuffer.texture;
			}
			else
			{
				// Started drawing to framebuffer.
				DebugGlCall(glBindFramebuffer(GL_FRAMEBUFFER, cache.current_framebuffer));
				DebugGlCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, entity.framebuffer.texture, 0));
			}
		}

		const glm::mat4 RotationMatrix = glm::toMat4(entity.rotation);
		const glm::mat4 ScaleMatrix = glm::scale(glm::identity<glm::mat4>(), glm::vec3(entity.scale));
		const glm::mat4 TranslationMatrix = glm::translate(glm::identity<glm::mat4>(), entity.position);

		// FIXME: This does not need to be updated every frame - possible optimization.
		const glm::mat4 model = TranslationMatrix * RotationMatrix * ScaleMatrix;
		DebugGlCall(glUniformMatrix4fv(uniform_locations.modelLoc, 1, GL_FALSE, glm::value_ptr(model)));

		if (vao.number_of_instances)
		{
			if (!vao.ibo) {
				// Instanced rendering
				DebugGlCall(glDrawArraysInstanced(GL_TRIANGLES, 0, vao.vertices_count, vao.number_of_instances));
			}
			else
			{
				// Indexed, instanced rendering:
				glDrawElementsInstanced(GL_TRIANGLES, vao.vertices_count, GL_UNSIGNED_INT, nullptr, vao.number_of_instances);
			}
		}
		else if (vao.ibo)
		{
			// Rendering with custom index buffer
			DebugGlCall(glDrawElements(GL_TRIANGLES, vao.vertices_count, GL_UNSIGNED_INT, nullptr));
		}
		else
		{
			// Rendering using automatic index buffer, which is consecutive numbers starting from 0.
			DebugGlCall(glDrawArrays(GL_TRIANGLES, 0, vao.vertices_count));
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

	RenderEntity e{};
	e.shader = entity.shader;
	e.id = id;
	e.vertex_array = entity.vertex_array;
	e.position = entity.position;
	e.rotation = entity.rotation;
	e.scale = entity.scale;
	e.environment = entity.environment;
	e.texture = entity.texture;

	render_entities.push_back(e);

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

void Renderer::invalidate_cache()
{
	cache.current_environment = 0;
	cache.current_shader = 0;
	cache.current_texture = 0;
	cache.current_framebuffer = 0;
}
