// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/GL/Buffer.hpp"

#if OPENGL_VERSION < OPENGL_VERSION_4_5
	#include "Engine/Adapter/Graphics/GL/BufferUsage.hpp"
#endif

namespace ProjectA {
	GL::Buffer::Buffer(const BufferDescriptor& descriptor) :
		ProjectA::Buffer(descriptor)
		#if OPENGL_VERSION < OPENGL_VERSION_4_5
			, target(getTarget(descriptor.usage))
		#endif
	{
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		#if OPENGL_VERSION >= OPENGL_VERSION_4_5
			glCreateBuffers(1, &buffer);
		#else
			glGenBuffers(1, &buffer);
			glBindBuffer(target, buffer);
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

		#if OPENGL_VERSION >= OPENGL_VERSION_4_5
			glNamedBufferStorage(buffer, size, nullptr, storageAccessFlags);
		#elif OPENGL_VERSION >= OPENGL_VERSION_4_4
			glBufferStorage(target, size, nullptr, storageAccessFlags);
		#else
			ENGINE_ERROR_UNSUPPORTED();
		#endif

		// Verify that the buffer has an immutable storage.
		#ifdef BUILD_MODE_DEBUG
			GLint immutableStorage = GL_FALSE;

			#if OPENGL_VERSION >= OPENGL_VERSION_4_5
				glGetNamedBufferParameteriv(buffer, GL_BUFFER_IMMUTABLE_STORAGE, &immutableStorage);
			#else
				glGetBufferParameteriv(target, GL_BUFFER_IMMUTABLE_STORAGE, &immutableStorage);
			#endif

			ENGINE_ASSERT(immutableStorage != GL_FALSE, "Only immutable buffer storage are not supported.");
		#endif

		#if OPENGL_VERSION < OPENGL_VERSION_4_5 && defined(OPENGL_UNBIND_AFTER_USE)
			glBindBuffer(target, 0);
		#endif
	}

	GL::Buffer::~Buffer() {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		glDeleteBuffers(1, &buffer);
	}

	void GL::Buffer::unmap() const {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		GLboolean result = GL_FALSE;

		#if OPENGL_VERSION >= OPENGL_VERSION_4_5
			result = glUnmapNamedBuffer(buffer);
		#else
			glBindBuffer(target, buffer);

			result = glUnmapBuffer(target);
		#endif

		ENGINE_ASSERT(result == GL_TRUE, "Failed to unmap OpenGL buffer.");
	}

	void* GL::Buffer::getMappedRange(uint64 offset, uint64 size) const {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		void* mappedRange = nullptr;

		#if OPENGL_VERSION >= OPENGL_VERSION_4_5
			mappedRange = glMapNamedBufferRange(buffer, offset, size, mapAccessFlags);
		#elif OPENGL_VERSION >= OPENGL_VERSION_3_0
			glBindBuffer(target, buffer);

			mappedRange = glMapBufferRange(target, offset, size, mapAccessFlags);
		#else
			ENGINE_ERROR_UNSUPPORTED();
		#endif

		ENGINE_ASSERT(mappedRange != nullptr, "Failed to map OpenGL buffer range.");

		return mappedRange;
	}
}