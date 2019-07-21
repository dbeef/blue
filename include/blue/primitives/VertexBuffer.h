#pragma once

#include "glad/glad.h"
#include "blue/Debugging.h"

//A Vertex Buffer Object(VBO) is the common term for a normal Buffer Object
//when it is used as a source for vertex array data. It is no different from 
//any other buffer object, and a buffer object used for Transform Feedback or 
//asynchronous pixel transfers can be used as source values for vertex arrays.
//
//This class wraps VBO and its functionality.
class VertexBuffer {
private:


public:

	GLuint _id;
	unsigned int _count;
	unsigned int _size;
	bool _initialized{};

	inline bool isInitialized() { return _initialized; }

	inline unsigned int getCount() const { return _count; }

	inline unsigned int getSize() const { return _size; }

	inline void init(unsigned int size) {
		_size = size;
		//_count = count;
		DebugGlCall(glGenBuffers(1, &_id));
		_initialized = true;
	}

	///Needs to be binded while calling.
	void copyDataToGpu(const void* data) {
		DebugGlCall(glBufferData(GL_ARRAY_BUFFER, _size, data, GL_STATIC_DRAW));
	}

	~VertexBuffer() {
		dispose();
	}

	inline void bind() const {
		DebugGlCall(glBindBuffer(GL_ARRAY_BUFFER, _id));
	}

	inline void unbind() const {
		DebugGlCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
	}

	void dispose() {
		if (_initialized) {
			DebugGlCall(glDeleteBuffers(1, &_id));
			_initialized = false;
		}
	}

};
