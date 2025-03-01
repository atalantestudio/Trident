#include "pch.hpp"

namespace ProjectA {
	/*OpenGLVertexBuffer::OpenGLVertexBuffer(uint32 size) {
		PROFILE_SCOPE(FUNC_SIG);

		glCreateBuffers(1, &object);

		bind();

		glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
	}

	OpenGLVertexBuffer::OpenGLVertexBuffer(float32* vertices, uint32 size) {
		PROFILE_SCOPE(FUNC_SIG);

		glCreateBuffers(1, &object);

		bind();

		glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
	}

	OpenGLVertexBuffer::~OpenGLVertexBuffer() {
		PROFILE_SCOPE(FUNC_SIG);

		glDeleteBuffers(1, &object);
	}

	void OpenGLVertexBuffer::copyData(const void* data, uint32 size) {
		bind();

		glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
	}

	void OpenGLVertexBuffer::bind() const {
		PROFILE_SCOPE(FUNC_SIG);

		glBindBuffer(GL_ARRAY_BUFFER, object);
	}

	void OpenGLVertexBuffer::unbind() const {
		PROFILE_SCOPE(FUNC_SIG);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}*/
}