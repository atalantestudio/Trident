#include "pch.hpp"

#include "Backend/OpenGL/OpenGLBuffer.hpp"

#ifndef OPENGL_USE_DSA
	#include "Backend/OpenGL/BufferUsage.hpp"
#endif

namespace ProjectA {
	OpenGLBuffer::OpenGLBuffer(const BufferDescriptor& descriptor) :
		Buffer(descriptor)
		#ifndef OPENGL_USE_DSA
			, target(getGlTarget(descriptor.usage))
		#endif
	{
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		#ifdef OPENGL_USE_DSA
			glCreateBuffers(1, &object);
		#else
			glGenBuffers(1, &object);
			glBindBuffer(target, object);
		#endif

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

		#ifdef OPENGL_USE_DSA
			glNamedBufferStorage(object, size, nullptr, storageAccessFlags);
		#else
			glBufferStorage(target, size, nullptr, storageAccessFlags);
		#endif
	}

	OpenGLBuffer::~OpenGLBuffer() {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		glDeleteBuffers(1, &object);
	}

	void OpenGLBuffer::unmap() const {
		GLboolean result;

		#ifdef OPENGL_USE_DSA
			result = glUnmapNamedBuffer(object);
		#else
			glBindBuffer(target, object);

			result = glUnmapBuffer(target);
		#endif

		ENGINE_ASSERT(result == GL_TRUE, "Failed to unmap OpenGL buffer.");
	}

	void* OpenGLBuffer::getMappedRange(uint64 offset, uint64 size) const {
		void* mappedRange;

		#ifdef OPENGL_USE_DSA
			mappedRange = glMapNamedBufferRange(object, offset, size, mapAccessFlags);
		#else
			glBindBuffer(target, object);

			mappedRange = glMapBufferRange(target, offset, size, mapAccessFlags);
		#endif

		ENGINE_ASSERT(mappedRange != nullptr, "Failed to map OpenGL buffer range.");

		return mappedRange;
	}
}