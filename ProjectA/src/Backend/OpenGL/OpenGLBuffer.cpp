#include "pch.hpp"

#include "Backend/OpenGL/BufferUsage.hpp"
#include "Backend/OpenGL/OpenGLBuffer.hpp"

namespace ProjectA {
	OpenGLBuffer::OpenGLBuffer(const BufferDescriptor& descriptor) :
		target(getGlBufferTarget(descriptor.usage)),
		size(descriptor.size)
	{
		PROFILE_SCOPE(FUNC_SIG);

		glCreateBuffers(1, &object);

		glBindBuffer(target, object);

		// TODO: Set correct usage flag.
		// GL_DYNAMIC_DRAW must be used with persistently mapped buffers,
		// such as a camera uniform buffer.
		glBufferData(target, size, nullptr, GL_DYNAMIC_DRAW);
	}

	OpenGLBuffer::~OpenGLBuffer() {
		PROFILE_SCOPE(FUNC_SIG);

		glDeleteBuffers(1, &object);
	}

	BufferMappedRange OpenGLBuffer::map(uint64 offset, uint64 size) const {
		glBindBuffer(target, object);

		// TODO: Set correct access flag.
		// GL_MAP_INVALIDATE_RANGE_BIT must be used with persistently mapped buffers,
		// such as a camera uniform buffer.
		return glMapBufferRange(target, offset, size, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT);
	}

	void OpenGLBuffer::unmap() const {
		glBindBuffer(target, object);

		if (!glUnmapBuffer(target)) {
			throw std::runtime_error("Failed to unmap buffer.");
		}
	}
}