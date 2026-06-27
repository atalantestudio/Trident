// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/GL/Buffer.hpp"

#if OPENGL_VERSION < OPENGL_VERSION_4_5
	#include "Engine/Adapter/Graphics/GL/BufferUsage.hpp"
#endif

#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace Trident {
	GL::BaseBuffer::BaseBuffer(const Trident::Device* device, const BufferDescriptor& descriptor) :
		Trident::BaseBuffer(device, descriptor)
	{}

	ref<Trident::BaseBuffer> GL::createBuffer(const Trident::Device* device, const BufferDescriptor& descriptor) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		ref<GL::BaseBuffer> buffer = createRef<GL::BaseBuffer>(device, descriptor);

		#if OPENGL_VERSION >= OPENGL_VERSION_4_5
			glCreateBuffers(1, &buffer->buffer);
		#else
			buffer->target = getTarget(descriptor.usage);

			glGenBuffers(1, &buffer->buffer);
			glBindBuffer(buffer->target, buffer->buffer);
		#endif

		if (
			(descriptor.memoryProperties & MemoryProperty::HOST_VISIBLE) ||
			(descriptor.memoryProperties & MemoryProperty::HOST_COHERENT) ||
			(descriptor.memoryProperties & MemoryProperty::HOST_CACHED)
		) {
			buffer->mapAccessFlags |= GL_MAP_WRITE_BIT;

			if (!descriptor.persistent) {
				buffer->storageAccessFlags |= GL_CLIENT_STORAGE_BIT;
			}
		}

		if (descriptor.persistent) {
			DEBUG_ASSERT(buffer->mapAccessFlags & GL_MAP_WRITE_BIT);

			buffer->mapAccessFlags |= GL_MAP_PERSISTENT_BIT;

			if (descriptor.memoryProperties & MemoryProperty::HOST_COHERENT) {
				buffer->mapAccessFlags |= GL_MAP_COHERENT_BIT;
			}
		}

		buffer->storageAccessFlags |= buffer->mapAccessFlags;

		#if OPENGL_VERSION >= OPENGL_VERSION_4_5
			glNamedBufferStorage(buffer->buffer, buffer->size, nullptr, buffer->storageAccessFlags);
		#elif OPENGL_VERSION >= OPENGL_VERSION_4_4
			glBufferStorage(buffer->target, buffer->size, nullptr, buffer->storageAccessFlags);
		#else
			ENGINE_ERROR_UNSUPPORTED();
		#endif

		// Verify that the buffer has an immutable storage.
		#if defined(BUILD_MODE_DEBUG)
			GLint immutableStorage = GL_FALSE;

			#if OPENGL_VERSION >= OPENGL_VERSION_4_5
				glGetNamedBufferParameteriv(buffer->buffer, GL_BUFFER_IMMUTABLE_STORAGE, &immutableStorage);
			#else
				glGetBufferParameteriv(buffer->target, GL_BUFFER_IMMUTABLE_STORAGE, &immutableStorage);
			#endif

			ADAPTER_ASSERT(immutableStorage != GL_FALSE, "Only immutable buffer storage are not supported.");
		#endif

		#if defined(DEBUG_OBJECT_NAME)
			if (descriptor.name.count() > 0) {
				glObjectLabel(GL_BUFFER, buffer->buffer, static_cast<GLsizei>(descriptor.name.count()), &descriptor.name[0]);
			}
		#endif

		return buffer;
	}

	void GL::destroyBuffer(const Trident::Device* device, const Trident::BaseBuffer* buffer) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const GL::BaseBuffer* _buffer = static_cast<const GL::BaseBuffer*>(buffer);

		glDeleteBuffers(1, &_buffer->buffer);
	}

	void* GL::mapBuffer(const Trident::Device* device, const Trident::BaseBuffer* buffer, uint64 offset, uint64 size) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const GL::BaseBuffer* _buffer = static_cast<const GL::BaseBuffer*>(buffer);
		void* mappedRange = nullptr;

		#if OPENGL_VERSION >= OPENGL_VERSION_4_5
			mappedRange = glMapNamedBufferRange(_buffer->buffer, offset, size, _buffer->mapAccessFlags);
		#elif OPENGL_VERSION >= OPENGL_VERSION_3_0
			glBindBuffer(_buffer->target, _buffer->buffer);

			mappedRange = glMapBufferRange(_buffer->target, offset, size, _buffer->mapAccessFlags);
		#else
			// TODO: Use glMapBuffer/glMapNamedBuffer.
			ENGINE_ERROR_UNSUPPORTED();
		#endif

		ADAPTER_ASSERT(mappedRange != nullptr, "Failed to map OpenGL buffer range.");

		return mappedRange;
	}

	void GL::unmapBuffer(const Trident::Device* device, const Trident::BaseBuffer* buffer) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const GL::BaseBuffer* _buffer = static_cast<const GL::BaseBuffer*>(buffer);
		GLboolean result = GL_FALSE;

		#if OPENGL_VERSION >= OPENGL_VERSION_4_5
			result = glUnmapNamedBuffer(_buffer->buffer);
		#else
			glBindBuffer(_buffer->target, _buffer->buffer);

			result = glUnmapBuffer(_buffer->target);
		#endif

		ADAPTER_ASSERT(result == GL_TRUE, "Failed to unmap OpenGL buffer.");
	}
}