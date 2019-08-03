#include <blue/gpu/handlers/CreateMeshHandler.hpp>
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
			blue::Context::logger().info("Vertex array created successfuly ({}).", vertex_array.vao);
		}
	}

	void set_vertex_buffer_layout(const std::vector<ShaderAttribute>& attributes, VertexBufferId vertex_buffer, IndexBufferId index_buffer) 
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
				DebugGlCall(glBindBuffer(GL_ARRAY_BUFFER, index_buffer));
				DebugGlCall(glVertexAttribPointer(index, attrib.getNumOfComponents(), GL_FLOAT, GL_FALSE, index_total_size, (GLvoid*)index_buffer_offset));
				
				// Set one instance of this attribute per vertex:
				DebugGlCall(glVertexAttribDivisor(index, 1));

				index_buffer_offset += attrib.getSize();
			}
			
			index++;
		}
	}

	VertexArray handle_no_index_buffer(const CreateMeshEntity& entity)
	{
		VertexBufferId vertex_buffer = 0;
		VertexArrayId vertex_array = 0;

		DebugGlCall(glGenBuffers(1, &vertex_buffer));

		DebugGlCall(glGenVertexArrays(1, &vertex_array));
		DebugGlCall(glBindVertexArray(vertex_array));

		DebugGlCall(glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer));
		DebugGlCall(glBufferData(GL_ARRAY_BUFFER, sizeof(VertexType) * entity.vertices.size(), entity.vertices.data(), GL_STATIC_DRAW));

		set_vertex_buffer_layout(entity.attributes, vertex_buffer, 0);
		
		glBindVertexArray(0);

		return VertexArray {vertex_array, vertex_buffer, 0, entity.indices_count};
	}

	VertexArray handle(const CreateMeshEntity& entity)
	{
		VertexBufferId vertex_buffer = 0;
		IndexBufferId index_buffer = 0;
		VertexArrayId vertex_array = 0;

		DebugGlCall(glGenBuffers(1, &vertex_buffer));
		DebugGlCall(glGenBuffers(1, &index_buffer));

		DebugGlCall(glGenVertexArrays(1, &vertex_array));
		DebugGlCall(glBindVertexArray(vertex_array));

		if (entity.number_of_instances)
		{
			// Use same type of buffer.
			DebugGlCall(glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer));
			DebugGlCall(glBufferData(GL_ARRAY_BUFFER, sizeof(VertexType) * entity.vertices.size(), entity.vertices.data(), GL_STATIC_DRAW));

			DebugGlCall(glBindBuffer(GL_ARRAY_BUFFER, index_buffer));
			DebugGlCall(glBufferData(GL_ARRAY_BUFFER, sizeof(IndexType) * entity.indices.size(), entity.indices.data(), GL_STATIC_DRAW));
		}
		else
		{
			DebugGlCall(glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer));
			DebugGlCall(glBufferData(GL_ARRAY_BUFFER, sizeof(VertexType) * entity.vertices.size(), entity.vertices.data(), GL_STATIC_DRAW));

			DebugGlCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer));
			DebugGlCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(IndexType) * entity.indices.size(), entity.indices.data(), GL_STATIC_DRAW));
		}

		set_vertex_buffer_layout(entity.attributes, vertex_buffer, index_buffer);

		glBindVertexArray(0);

		return VertexArray{ vertex_array, vertex_buffer, index_buffer, entity.indices_count};
	}
}

void handle(std::pair<std::promise<VertexArray>, CreateMeshEntity>& pair)
{
	std::promise<VertexArray>& promise = pair.first;
	const CreateMeshEntity& entity = pair.second;

	if (entity.indices.empty())
	{
		VertexArray vao = handle_no_index_buffer(entity);
		promise.set_value(vao);
		log_status(vao);
	}
	else
	{
		VertexArray vao = handle(entity);
		vao.number_of_instances = entity.number_of_instances;
		promise.set_value(vao);
		log_status(vao);
	}
}
