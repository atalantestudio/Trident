#include "pch.hpp"
#include "OpenGLVertexBuffer.hpp"
#include <glad/glad.h>

namespace ProjectA {
	OpenGLVertexBuffer::OpenGLVertexBuffer(float* vertices, uint32_t size) {
		glCreateBuffers(1, &id);

		bind();

		glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
	}

	OpenGLVertexBuffer::~OpenGLVertexBuffer() {
		glDeleteBuffers(1, &id);
	}

	const BufferLayout& OpenGLVertexBuffer::getLayout() const {
		return layout;
	}

	void OpenGLVertexBuffer::setLayout(const BufferLayout& layout) {
		this->layout = layout;
	}

	void OpenGLVertexBuffer::bind() const {
		glBindBuffer(GL_ARRAY_BUFFER, id);
	}

	void OpenGLVertexBuffer::unbind() const {
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}