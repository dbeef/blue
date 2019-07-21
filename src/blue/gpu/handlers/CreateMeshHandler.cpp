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

	void set_vertex_buffer_layout(const std::vector<ShaderAttribute>& attributes) 
	{
		int total_size = 0;
		for (const auto& attrib : attributes)
		{
			total_size += attrib.getSize();
		}

		int index = 0;
		int offset = 0;
		for (const auto& attrib : attributes)
		{
			DebugGlCall(glVertexAttribPointer(index, attrib.getNumOfComponents(), GL_FLOAT, GL_FALSE, total_size, (GLvoid*)offset));
			DebugGlCall(glEnableVertexAttribArray(index));
			index++;
			offset += attrib.getSize();
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

		set_vertex_buffer_layout(entity.attributes);
		
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

		DebugGlCall(glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer));
		DebugGlCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer));

		DebugGlCall(glBufferData(GL_ARRAY_BUFFER, sizeof(VertexType) * entity.vertices.size(), entity.vertices.data(), GL_STATIC_DRAW));
		DebugGlCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(IndexType) * entity.indices.size(), entity.indices.data(), GL_STATIC_DRAW));

		set_vertex_buffer_layout(entity.attributes);

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
		promise.set_value(vao);
		log_status(vao);
	}
}
