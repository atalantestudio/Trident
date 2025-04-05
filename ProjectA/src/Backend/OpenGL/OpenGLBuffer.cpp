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

		// TODO: Provide mapping info, such as persistent mapping, when creating the buffer.

		// TODO: Set correct usage flag.
		// GL_DYNAMIC_DRAW must be used with persistently mapped buffers,
		// such as a camera uniform buffer.
		glNamedBufferData(object, size, nullptr, GL_DYNAMIC_DRAW);
	}

	OpenGLBuffer::~OpenGLBuffer() {
		PROFILE_SCOPE(FUNC_SIG);

		glDeleteBuffers(1, &object);
	}

	void OpenGLBuffer::unmap() const {
		ENGINE_ASSERT(glUnmapNamedBuffer(object), "Failed to unmap OpenGL buffer.");
	}

	void* OpenGLBuffer::getMappedRange(uint64 offset, uint64 size) const {
		// TODO: Set correct access flag.
		// GL_MAP_INVALIDATE_RANGE_BIT must be used with persistently mapped buffers,
		// such as a camera uniform buffer.
		void* mappedRange = glMapNamedBufferRange(object, offset, size, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT);

		ENGINE_ASSERT(mappedRange != nullptr, "Failed to map OpenGL buffer range.");

		return mappedRange;
	}
}