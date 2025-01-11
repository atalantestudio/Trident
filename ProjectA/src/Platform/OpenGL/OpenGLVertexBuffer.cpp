#include "pch.hpp"

#include <glad/glad.h>

#include "OpenGLVertexBuffer.hpp"

namespace ProjectA {
	OpenGLVertexBuffer::OpenGLVertexBuffer(float* vertices, uint32_t size) {
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