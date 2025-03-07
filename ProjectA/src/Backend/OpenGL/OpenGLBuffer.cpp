#include "pch.hpp"

#include "Backend/OpenGL/BufferUsage.hpp"
#include "Backend/OpenGL/OpenGLBuffer.hpp"

namespace ProjectA {
	OpenGLBuffer::OpenGLBuffer(const BufferDescriptor& descriptor) :
		target(getGlBufferTarget(descriptor.usage)),
		size(descriptor.size)
	{
		PROFILE_SCOPE(FUNC_SIG);

		ENGINE_TRACE("[OpenGLBuffer]");

		glCreateBuffers(1, &object);

		glBindBuffer(target, object);

		// TODO: Set correct usage.
		glBufferData(target, size, nullptr, GL_STATIC_DRAW);

		glBindBuffer(target, 0);
	}

	OpenGLBuffer::~OpenGLBuffer() {
		PROFILE_SCOPE(FUNC_SIG);

		ENGINE_TRACE("[~OpenGLBuffer]");

		glDeleteBuffers(1, &object);
	}

	void OpenGLBuffer::map(const uint64 offset, const uint64 size) {}

	void OpenGLBuffer::write(const void* data, const uint64 offset, const uint64 size) {
		glBindBuffer(target, object);

		glBufferSubData(target, offset, size, data);

		glBindBuffer(target, 0);
	}

	void OpenGLBuffer::unmap() {}
}