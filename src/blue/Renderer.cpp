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
    for (const auto &pair : imgui_entities)
    {
        const auto &imgui_entity = pair.second;
        imgui_entity();
    }
}

void Renderer::draw_render_entities()
{
    for (const auto &entity : render_entities)
    {
        const auto &vao = entity.vertex_array;
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
            DebugGlCall(glBindBufferRange(GL_UNIFORM_BUFFER, 0, cache.current_environment, 0, 256));
        }

        // If entity utilizes texture (and only one is allowed to be passed in an entity, at least for now):
        for (std::size_t index = 0; index < BLUE_AVAILABLE_TEXTURE_SLOTS; index++)
        {
            const auto& texture = entity.textures[index];

            if (texture.id != 0)
            {
                auto requestedSlot = cache.textures.find(texture.slot);
                if (requestedSlot != cache.textures.end())
                {
                    if (requestedSlot->second != texture.id)
                    {
                        requestedSlot->second = texture.id;
                        DebugGlCall(glActiveTexture(GL_TEXTURE0 + texture.slot));
                        DebugGlCall(glBindTexture(GL_TEXTURE_2D, texture.id));
                    }
                }
                else
                {
                    cache.textures.insert({texture.slot, texture.id});
                    DebugGlCall(glActiveTexture(GL_TEXTURE0 + texture.slot));
                    DebugGlCall(glBindTexture(GL_TEXTURE_2D, texture.id));
                }
            }
        }

        if (cache.current_framebuffer != entity.framebuffer.framebuffer)
        {
            cache.current_framebuffer = entity.framebuffer.framebuffer;
            if (cache.current_framebuffer == 0)
            {
                // Finished drawing to framebuffer:
                DebugGlCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
                DebugGlCall(glViewport(0, 0, blue::Context::window().get_width(), blue::Context::window().get_height()));
            }
            else
            {
                // Started drawing to framebuffer.
                DebugGlCall(glViewport(0, 0, entity.framebuffer.texture.width, entity.framebuffer.texture.height));
                DebugGlCall(glBindFramebuffer(GL_FRAMEBUFFER, cache.current_framebuffer));
                DebugGlCall(glActiveTexture(GL_TEXTURE0 + entity.framebuffer.texture.slot));
                DebugGlCall(glBindTexture(GL_TEXTURE_2D, entity.framebuffer.texture.id));
				// FIXME: This needs different GL_*_BUFFER_BIT, depending on texture attachment.
                DebugGlCall(glClear(GL_DEPTH_BUFFER_BIT));
                set_cached_texture(entity.framebuffer.texture.slot, entity.framebuffer.texture.id);
            }
        }

        const glm::mat4 RotationMatrix = glm::toMat4(entity.rotation);
        const glm::mat4 ScaleMatrix = glm::scale(glm::identity<glm::mat4>(), entity.scale);
        const glm::mat4 TranslationMatrix = glm::translate(glm::identity<glm::mat4>(), entity.position);

        // FIXME: This does not need to be updated every frame - possible optimization.
        const glm::mat4 model = TranslationMatrix * RotationMatrix * ScaleMatrix;
        DebugGlCall(glUniformMatrix4fv(uniform_locations.modelLoc, 1, GL_FALSE, glm::value_ptr(model)));

        if (vao.number_of_instances)
        {
            if (!vao.ibo)
            {
                // Instanced rendering
                DebugGlCall(glDrawArraysInstanced(GL_TRIANGLES, 0, vao.vertices_count, vao.number_of_instances));
            }
            else
            {
                // Indexed, instanced rendering:
                glDrawElementsInstanced(GL_TRIANGLES, vao.vertices_count, GL_UNSIGNED_INT, nullptr,
                                        vao.number_of_instances);
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

ImGuiEntityId Renderer::add(const ImGuiEntity &entity)
{
    lock();
    static ImGuiEntityId id = 1;
    id++;
    imgui_entities.emplace_back(std::make_pair(id, entity));
    unlock();
    return id;
}

RenderEntityId Renderer::add(const RenderEntity &entity)
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
    std::memcpy(&e.textures[0], &entity.textures[0], sizeof(RenderEntity::textures));

    {
        e.framebuffer.framebuffer = entity.framebuffer.framebuffer;
        e.framebuffer.texture = entity.framebuffer.texture;
    }
    render_entities.push_back(e);

    sort_entities_by_shader();
    sort_entities_by_framebuffer();
    unlock();
    return id;
}

void Renderer::update(const RenderEntity &entity)
{
    lock();
    auto iterator = std::find_if(render_entities.begin(), render_entities.end(),
                                 [entity](const RenderEntity &list_entity) -> bool
                                 {
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

void Renderer::remove_render_entity(const RenderEntityId &id)
{
    lock();
    auto iterator = std::remove_if(render_entities.begin(), render_entities.end(),
                                   [id](const RenderEntity &list_entity) -> bool
                                   {
                                       return list_entity.id == id;
                                   });

    if (iterator != render_entities.end())
    {
        render_entities.erase(iterator);
    }
    unlock();
}

void Renderer::remove_imgui_entity(const ImGuiEntityId &id)
{
    lock();
    auto iterator = std::remove_if(imgui_entities.begin(), imgui_entities.end(),
                                   [id](const std::pair<ImGuiEntityId, ImGuiEntity> &list_entity) -> bool
                                   {
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

void Renderer::unlock()
{
    locked.store(false);
}

void Renderer::sort_entities_by_shader()
{
    std::sort(render_entities.begin(), render_entities.end(),
              [](const RenderEntity &first, const RenderEntity &second)
              {
                  return first.shader > second.shader;
              });
}

void Renderer::sort_entities_by_framebuffer()
{
    std::sort(render_entities.begin(), render_entities.end(),
              [](const RenderEntity &first, const RenderEntity &second)
              {
                  return first.framebuffer.framebuffer > second.framebuffer.framebuffer;
              });
}

void Renderer::invalidate_cache_uniform_buffer()
{
    cache.current_environment = 0;
}

ShaderId Renderer::get_cached_shader() const
{
    return cache.current_shader;
}

void Renderer::set_cached_shader(ShaderId program)
{
    cache.current_shader = program;
}

std::map<TextureSlot, TextureId> &Renderer::get_cached_textures()
{
    return cache.textures;
}

void Renderer::set_cached_framebuffer(FramebufferId framebufferId)
{
    cache.current_framebuffer = framebufferId;
}

void Renderer::set_cached_texture(TextureSlot textureSlot, TextureId textureId)
{
    auto mapEntry = cache.textures.find(textureSlot);
    if (mapEntry != cache.textures.end())
    {
        mapEntry->second = textureId;
    }
    else
    {
        cache.textures.insert({textureSlot, textureId});
    }
}
