#include <blue/gpu/handlers/CreateMeshHandler.hpp>
#include <blue/gpu/GpuEntities.hpp>
#include "blue/Context.hpp"
#include <blue/gpu/handlers/CreateMeshHandler.hpp>
#include <blue/gpu/GpuEntities.hpp>
#include "blue/Context.hpp"

namespace
{
	void log_status(const VertexArray& vertex_array)
	{
		if (vertex_array.vao == 0)
		{
			blue::Context::logger().error("Failed to create vertex array.");
		}
		else
		{
			blue::Context::logger().info("Vertex array created successfuly ({}, VBO: {} IBO: {}).",
				vertex_array.vao, vertex_array.vbo, vertex_array.ibo);
		}
	}

	void set_vertex_buffer_layout(const std::vector<ShaderAttribute>& attributes, VertexBufferId vertex_buffer, InstanceBufferId instance_buffer)
	{
		int vertex_total_size = 0;
		int index_total_size = 0;
		for (const auto& attrib : attributes)
		{
			if (attrib._buffer == ShaderAttribute::Buffer::VERTEX)
			{
				vertex_total_size += attrib.getSize();
			}
			else
			{
				index_total_size += attrib.getSize();
			}
		}

		int index = 0;
		int vertex_buffer_offset = 0;
		int index_buffer_offset = 0;

		for (const auto& attrib : attributes)
		{
			DebugGlCall(glEnableVertexAttribArray(index));

			if (attrib._buffer == ShaderAttribute::Buffer::VERTEX)
			{
				DebugGlCall(glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer));
				DebugGlCall(glVertexAttribPointer(index, attrib.getNumOfComponents(), GL_FLOAT, GL_FALSE, vertex_total_size, (GLvoid*)vertex_buffer_offset));

				vertex_buffer_offset += attrib.getSize();
			}
			else
			{
				DebugGlCall(glBindBuffer(GL_ARRAY_BUFFER, instance_buffer));
				DebugGlCall(glVertexAttribPointer(index, attrib.getNumOfComponents(), GL_FLOAT, GL_FALSE, index_total_size, (GLvoid*)index_buffer_offset));

				// Tell OpenGL this is an instanced vertex attribute.
				DebugGlCall(glVertexAttribDivisor(index, 1));

				index_buffer_offset += attrib.getSize();
			}

			index++;
		}
	}

	VertexArray handle(const CreateInstancedMeshEntity& entity)
	{
		VertexArrayId vertex_array = 0;

		DebugGlCall(glGenVertexArrays(1, &vertex_array));
		DebugGlCall(glBindVertexArray(vertex_array));

		DebugGlCall(glBindBuffer(GL_ARRAY_BUFFER, entity.vao.vbo));
		DebugGlCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, entity.vao.ibo));

		InstanceBufferId instance_buffer = 0;
		DebugGlCall(glGenBuffers(1, &instance_buffer));
		DebugGlCall(glBindBuffer(GL_ARRAY_BUFFER, instance_buffer));
		DebugGlCall(glBufferData(GL_ARRAY_BUFFER, sizeof(IndexType) * entity.instances.size(), entity.instances.data(), GL_STATIC_DRAW));
		set_vertex_buffer_layout(entity.attributes, entity.vao.vbo, instance_buffer);
		glBindVertexArray(0);

		VertexArray a{};
		a.vao = vertex_array;
		a.vbo = entity.vao.vbo;
		a.ibo = entity.vao.ibo;
		a.vertices_count = entity.vao.vertices_count;
		a.number_of_instances = static_cast<std::uint32_t>(entity.instances.size());

		return a;
	}
}

void handle(std::pair<std::promise<VertexArray>, CreateInstancedMeshEntity>& pair)
{
	std::promise<VertexArray>& promise = pair.first;
	const CreateInstancedMeshEntity& entity = pair.second;

	VertexArray vao = handle(entity);
	promise.set_value(vao);
	log_status(vao);
}
