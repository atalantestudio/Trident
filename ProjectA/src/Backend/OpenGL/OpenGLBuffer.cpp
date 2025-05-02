#include "pch.hpp"

#include "Backend/OpenGL/BufferUsage.hpp"
#include "Backend/OpenGL/OpenGLBuffer.hpp"

namespace ProjectA {
	OpenGLBuffer::OpenGLBuffer(const BufferDescriptor& descriptor) :
		Buffer(descriptor.size),
		target(getGlBufferTarget(descriptor.usage))
	{
		PROFILE_SCOPE(FUNC_SIG);

		glCreateBuffers(1, &object);

		access = GL_MAP_WRITE_BIT;

		if (descriptor.persistent) {
			access |= GL_MAP_PERSISTENT_BIT;
		}

		glNamedBufferStorage(object, size, nullptr, access);
	}

	OpenGLBuffer::~OpenGLBuffer() {
		PROFILE_SCOPE(FUNC_SIG);

		glDeleteBuffers(1, &object);
	}

	void OpenGLBuffer::unmap() const {
		ENGINE_ASSERT(glUnmapNamedBuffer(object), "Failed to unmap OpenGL buffer.");
	}

	void* OpenGLBuffer::getMappedRange(uint64 offset, uint64 size) const {
		void* mappedRange = glMapNamedBufferRange(object, offset, size, access);

		ENGINE_ASSERT(mappedRange != nullptr, "Failed to map OpenGL buffer range.");

		return mappedRange;
	}
}