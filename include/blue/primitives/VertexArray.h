//#pragma once
//
//#include "glad/glad.h"
//#include <iostream>
//#include <vector>
//#include "blue/Debugging.h"
//#include "VertexBuffer.h"
//#include "IndexBuffer.h"
//
////A Vertex Array Object (VAO) is an OpenGL Object that stores all of the
////state needed to supply vertex data. It stores the format of the vertex data
////as well as the Buffer Objects providing the vertex data arrays. Note that VAOs
////do not copy, freeze or store the contents of the referenced buffers -
////if you change any of the data in the buffers referenced by an existing VAO,
////those changes will be seen by users of the VAO.
////
////This class wraps VAO and its functionality.
//
//
//struct ShaderAttribute
//{
//	enum class Type 
//	{
//		FLOAT,
//		VEC2,
//		VEC3,
//		VEC4
//	};
//
//	ShaderAttribute(Type type, const std::string& name) : _type(type), _name(name) {}
//
//	std::size_t getNumOfComponents() const
//	{
//		switch (_type)
//		{
//		case Type::FLOAT: return 1;
//		case Type::VEC2: return 2;
//		case Type::VEC3: return 3;
//		case Type::VEC4: return 4;
//		}
//	}
//
//	std::size_t getSize() const
//	{
//		return sizeof(float) * getNumOfComponents();
//	}
//
//private:
//
//	const Type _type;
//	const std::string _name;
//};
//
//namespace blue
//{
//	using VertexType = float;
//	using IndexType = unsigned int;
//
//	using Vertices = std::vector<VertexType>;
//	using Indices = std::vector<IndexType>;
//	using Attributes = std::vector<ShaderAttribute>;
//
//	//class MeshSource
//	//{
//	//public:
//
//	//	MeshSource(
//	//		const Vertices& vertices,
//	//		const Indices& indices,
//	//		const Attributes& attributes)
//	//		: _vertices(vertices), _indices(indices), _attributes(attributes) {}
//
//	//	inline const Vertices vertices() const { return _vertices; };
//	//	inline const Indices indices() const { return _indices; };
//	//	inline const Attributes attributes() const { return _attributes; };
//	//	
//	//private:
//
//	//	const Vertices& _vertices;
//	//	const Indices& _indices;
//	//	const Attributes& _attributes;
//	//};
//}
//
//class VertexArray {
//
//public:
//
//	GLuint _id;
//	VertexBuffer _vbo;
//	IndexBuffer _ibo;
//
//	inline bool isInitialized() const {
//		return _id != 0;
//	}
//
//	inline GLuint getId() const {
//		return _id;
//	}
//
//	void init(const blue::Vertices& vertices, int vertex_count, const blue::Attributes& attributes)
//	{
//		init(vertices.data(), sizeof(blue::VertexType) * vertices.size(), attributes);
//	}
//
//	void init(const blue::Vertices& vertices, const blue::Indices& indices, const blue::Attributes& attributes)
//	{
//		init(vertices.data(), sizeof(blue::VertexType) * vertices.size(),
//			indices.data(), sizeof(blue::IndexType) * indices.size(), indices.size(), attributes);
//	}
//
//	void init(const void* vertex_data, unsigned int vertex_data_size, const void* index_data, unsigned int index_data_size, unsigned int index_count, const std::vector<ShaderAttribute>& attributes) {
//
//		if (isInitialized()) dispose();
//
//		_vbo.init(vertex_data_size);
//		_ibo.init(index_data_size, index_count);
//
//		glGenVertexArrays(1, &_id);
//		glBindVertexArray(_id);
//		{
//
//			//bind _vbo & _ibo to VBA and set its properties
//			_vbo.bind();
//			_ibo.bind();
//
//			_vbo.copyDataToGpu(vertex_data); //must be called here, where it is binded to the VBA.
//			_ibo.copyDataToGpu(index_data);  //must be called here, where it is binded to the VBA.
//
//			int total_size = 0;
//			for (const auto& attrib : attributes)
//			{
//				total_size += attrib.getSize();
//			}
//
//			int index = 0;
//			int offset = 0;
//			for (const auto& attrib : attributes)
//			{
//				DebugGlCall(glVertexAttribPointer(index, attrib.getNumOfComponents(), GL_FLOAT, GL_FALSE, total_size, (GLvoid*)offset));
//				DebugGlCall(glEnableVertexAttribArray(index));
//				index++;
//				offset += attrib.getSize();
//			}
//
//			_vbo.unbind();
//
//		}
//		glBindVertexArray(0);
//
//	}
//
//	void init(const void* vertex_data, unsigned int vertex_data_size, const std::vector<ShaderAttribute>& attributes) {
//
//		if (isInitialized()) dispose();
//
//		_vbo.init(vertex_data_size);
//
//		DebugGlCall(glGenVertexArrays(1, &_id));
//		DebugGlCall(glBindVertexArray(_id));
//
//		{
//			//bind _vbo & _ibo to VBA and set its properties
//			_vbo.bind();
//			_vbo.copyDataToGpu(vertex_data); //must be called here, where it is binded to the VBA.
//
//			int total_size = 0;
//			for (const auto& attrib : attributes)
//			{
//				total_size += attrib.getSize();
//			}
//
//			int index = 0;
//			int offset = 0;
//			for (const auto& attrib : attributes)
//			{
//				DebugGlCall(glVertexAttribPointer(index, attrib.getNumOfComponents(), GL_FLOAT, GL_FALSE, total_size, (GLvoid*)offset));
//				DebugGlCall(glEnableVertexAttribArray(index));
//				index++;
//				offset += attrib.getSize();
//			}
//
//			_vbo.unbind();
//		}
//		DebugGlCall(glBindVertexArray(0));
//	}
//
//	inline void render() {
//		if (_ibo.isInitialized()) {
//			DebugGlCall(glBindVertexArray(_id));
//			DebugGlCall(glDrawElements(GL_TRIANGLES, _ibo.getCount(), GL_UNSIGNED_INT, 0));
//			DebugGlCall(glBindVertexArray(0));
//		}
//		else
//		{
//			//glDrawArrays does not utilize index buffers (it uses consecutive numbers starting from 0 to fill its index buffer)
//			//it's probably more suitable for 3d models (why?)
//			//you still need to know what's the size of vertices
//			DebugGlCall(glBindVertexArray(_id));
//			DebugGlCall(glDrawArrays(GL_TRIANGLES, 0, _vbo.getCount()));
//			DebugGlCall(glBindVertexArray(0));
//		}
//	}
//
//	~VertexArray() {
//		dispose();
//	}
//
//	void dispose() {
//		_vbo.dispose();
//		_ibo.dispose();
//		DebugGlCall(glDeleteBuffers(1, &_id));
//		_id = 0;
//	}
//
//};
