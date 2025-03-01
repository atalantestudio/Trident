#include "pch.hpp"

namespace ProjectA {
	/*OpenGLIndexBuffer::OpenGLIndexBuffer(const uint32* indices, uint32 count) : count(count) {
		PROFILE_SCOPE(FUNC_SIG);

		glCreateBuffers(1, &object);

		bind();

		glBindBuffer(GL_ARRAY_BUFFER, object);
		glBufferData(GL_ARRAY_BUFFER, sizeof(uint32) * count, indices, GL_STATIC_DRAW);
	}

	OpenGLIndexBuffer::~OpenGLIndexBuffer() {
		PROFILE_SCOPE(FUNC_SIG);

		glDeleteBuffers(1, &object);
	}

	uint32 OpenGLIndexBuffer::getCount() const {
		return count;
	}

	void OpenGLIndexBuffer::bind() const {
		PROFILE_SCOPE(FUNC_SIG);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, object);
	}

	void OpenGLIndexBuffer::unbind() const {
		PROFILE_SCOPE(FUNC_SIG);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}*/
}