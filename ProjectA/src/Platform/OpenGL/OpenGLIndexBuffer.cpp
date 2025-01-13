#include "pch.hpp"

#include <glad/glad.h>

#include "Platform/OpenGL/OpenGLIndexBuffer.hpp"

namespace ProjectA {
	OpenGLIndexBuffer::OpenGLIndexBuffer(const uint32_t* indices, uint32_t count) : count(count) {
		PROFILE_SCOPE(__FUNCSIG__);

		glCreateBuffers(1, &id);

		bind();

		glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
	}

	OpenGLIndexBuffer::~OpenGLIndexBuffer() {
		PROFILE_SCOPE(__FUNCSIG__);

		glDeleteBuffers(1, &id);
	}

	uint32_t OpenGLIndexBuffer::getCount() const {
		return count;
	}

	void OpenGLIndexBuffer::bind() const {
		PROFILE_SCOPE(__FUNCSIG__);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
	}

	void OpenGLIndexBuffer::unbind() const {
		PROFILE_SCOPE(__FUNCSIG__);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}