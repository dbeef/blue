#pragma once

#include "glad/glad.h"
#include "blue/Debugging.h"

//Indexed rendering requires an array of indices; all vertex attributes
//will use the same index from this index array. The index array is provided
//by a Buffer Object bound to the GL_ELEMENT_ARRAY_BUFFER binding point.
//When a buffer is bound to GL_ELEMENT_ARRAY_BUFFER, all drawing commands of
//the form gl*Draw*Elements* will use indexes from that buffer. Indices can be
//unsigned bytes, unsigned shorts, or unsigned ints.
//The index buffer binding is stored within the VAO. If no VAO is bound, then you
//cannot bind a buffer object to GL_ELEMENT_ARRAY_BUFFER.
//
//This class wraps IBO and its functionalities.
//IBO is sometimes called EBO (element buffer object).
class IndexBuffer {

private:
public:
	GLuint _id{};
	unsigned int _count{};
	unsigned int _data_size{};
	bool _initialized{};

	inline bool isInitialized() const { return _initialized; }

	inline void init(unsigned int data_size, unsigned int count) {
		_count = count;
		_data_size = data_size;
		DebugGlCall(glGenBuffers(1, &_id));
		_initialized = true;
	}

	inline unsigned int getCount() const {
		return _count;
	}

	///Needs to be binded while calling.
	void copyDataToGpu(const void* indices) {
		DebugGlCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, _data_size, indices, GL_STATIC_DRAW));
	}

	~IndexBuffer() {
		dispose();
	}

	inline void bind() const {
		DebugGlCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _id));
	}

	inline void unbind() const {
		DebugGlCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
	}

	void dispose(){
		if (_initialized) {
			DebugGlCall(glDeleteBuffers(1, &_id));
			_initialized = false;
		}
	}

};
