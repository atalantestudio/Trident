#include "pch.hpp"

#include "Backend/OpenGL/OpenGLBuffer.hpp"

namespace ProjectA {
	OpenGLBuffer::OpenGLBuffer(const BufferDescriptor& descriptor) :
		Buffer(descriptor.size)
	{
		PROFILE_SCOPE(FUNC_SIG);

		glCreateBuffers(1, &object);

		if (
			(descriptor.memoryProperties & MemoryProperty::HOST_VISIBLE) ||
			(descriptor.memoryProperties & MemoryProperty::HOST_COHERENT) ||
			(descriptor.memoryProperties & MemoryProperty::HOST_CACHED)
		) {
			mapAccessFlags |= GL_MAP_WRITE_BIT;

			if (!descriptor.persistent) {
				storageAccessFlags |= GL_CLIENT_STORAGE_BIT;
			}
		}

		if (descriptor.persistent) {
			DEBUG_ASSERT(mapAccessFlags & GL_MAP_WRITE_BIT);

			mapAccessFlags |= GL_MAP_PERSISTENT_BIT;

			if (descriptor.memoryProperties & MemoryProperty::HOST_COHERENT) {
				mapAccessFlags |= GL_MAP_COHERENT_BIT;
			}
		}

		storageAccessFlags |= mapAccessFlags;

		glNamedBufferStorage(object, size, nullptr, storageAccessFlags);
	}

	OpenGLBuffer::~OpenGLBuffer() {
		PROFILE_SCOPE(FUNC_SIG);

		glDeleteBuffers(1, &object);
	}

	void OpenGLBuffer::unmap() const {
		ENGINE_ASSERT(glUnmapNamedBuffer(object), "Failed to unmap OpenGL buffer.");
	}

	void* OpenGLBuffer::getMappedRange(uint64 offset, uint64 size) const {
		void* mappedRange = glMapNamedBufferRange(object, offset, size, mapAccessFlags);

		ENGINE_ASSERT(mappedRange != nullptr, "Failed to map OpenGL buffer range.");

		return mappedRange;
	}
}