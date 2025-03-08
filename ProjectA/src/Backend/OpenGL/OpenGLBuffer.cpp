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

		// TODO: Set correct usage flag.
		glBufferData(target, size, nullptr, GL_STATIC_DRAW);
	}

	OpenGLBuffer::~OpenGLBuffer() {
		PROFILE_SCOPE(FUNC_SIG);

		ENGINE_TRACE("[~OpenGLBuffer]");

		glDeleteBuffers(1, &object);
	}

	BufferMappedRange OpenGLBuffer::map(uint64 offset, uint64 size) const {
		glBindBuffer(target, object);

		// TODO: Set correct access flag.
		return glMapBuffer(target, GL_WRITE_ONLY);
	}

	/*void OpenGLBuffer::write(const void* data, const uint64 offset, const uint64 size) {
		glBindBuffer(target, object);

		glBufferSubData(target, offset, size, data);
	}*/

	void OpenGLBuffer::unmap() const {
		glBindBuffer(target, object);

		glUnmapBuffer(target);

		glBindBuffer(target, 0);
	}
}