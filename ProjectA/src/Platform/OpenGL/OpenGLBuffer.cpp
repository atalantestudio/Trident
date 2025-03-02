#include "pch.hpp"

#include "Platform/OpenGL/BufferUsage.hpp"
#include "Platform/OpenGL/OpenGLBuffer.hpp"

namespace ProjectA {
	// TODO: Write correct binding.
	OpenGLBuffer::OpenGLBuffer(const BufferDescriptor& descriptor) : binding(getGlBufferBinding(descriptor.usage)), size(descriptor.size) {
		PROFILE_SCOPE(FUNC_SIG);

		glCreateBuffers(1, &object);
	}

	OpenGLBuffer::~OpenGLBuffer() {
		PROFILE_SCOPE(FUNC_SIG);

		glDeleteBuffers(1, &object);
	}

	void OpenGLBuffer::write(const void* data, const std::size_t size) const {
		glBindBuffer(binding, object);

		glBufferData(binding, size, data, GL_STATIC_DRAW);

		glBindBuffer(binding, 0);
	}
}