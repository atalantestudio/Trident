#include "pch.hpp"

#include <glad/glad.h>

#include "Platform/OpenGL/OpenGLVertexBuffer.hpp"

namespace ProjectA {
	OpenGLVertexBuffer::OpenGLVertexBuffer(uint32_t size) {
		PROFILE_SCOPE(__FUNCSIG__);

		glCreateBuffers(1, &id);

		bind();

		glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
	}

	OpenGLVertexBuffer::OpenGLVertexBuffer(float32* vertices, uint32_t size) {
		PROFILE_SCOPE(__FUNCSIG__);

		glCreateBuffers(1, &id);

		bind();

		glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
	}

	OpenGLVertexBuffer::~OpenGLVertexBuffer() {
		PROFILE_SCOPE(__FUNCSIG__);

		glDeleteBuffers(1, &id);
	}

	const BufferLayout& OpenGLVertexBuffer::getLayout() const {
		return layout;
	}

	void OpenGLVertexBuffer::copyData(const void* data, uint32 size) {
		bind();

		glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
	}

	void OpenGLVertexBuffer::setLayout(const BufferLayout& layout) {
		this->layout = layout;
	}

	void OpenGLVertexBuffer::bind() const {
		PROFILE_SCOPE(__FUNCSIG__);

		glBindBuffer(GL_ARRAY_BUFFER, id);
	}

	void OpenGLVertexBuffer::unbind() const {
		PROFILE_SCOPE(__FUNCSIG__);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}