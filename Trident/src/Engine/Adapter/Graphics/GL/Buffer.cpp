// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/GL/Buffer.hpp"
#include "Engine/Adapter/Graphics/GL/BufferUsage.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace Trident {
	GL::Buffer::Buffer(const Trident::Device* device, const BufferDescriptor& descriptor) :
		Trident::Buffer(device, descriptor)
	{}

	ref<Trident::Buffer> GL::createBuffer(const Trident::Device* device, const BufferDescriptor& descriptor) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		ref<GL::Buffer> buffer = createRef<GL::Buffer>(device, descriptor);
		buffer->target = getTarget(descriptor.usage);

		glGenBuffers(1, &buffer->buffer);
		glBindBuffer(buffer->target, buffer->buffer);

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

			glGetBufferParameteriv(buffer->target, GL_BUFFER_IMMUTABLE_STORAGE, &immutableStorage);

			ADAPTER_ASSERT(immutableStorage != GL_FALSE, "Only immutable buffer storage are not supported.");
		#endif

		#if defined(DEBUG_OBJECT_NAME)
			if (descriptor.name.count() > 0) {
				glObjectLabel(GL_BUFFER, buffer->buffer, static_cast<GLsizei>(descriptor.name.count()), &descriptor.name[0]);
			}
		#endif

		return buffer;
	}

	void GL::destroyBuffer(const Trident::Device* device, const Trident::Buffer* buffer) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const GL::Buffer* _buffer = static_cast<const GL::Buffer*>(buffer);

		glDeleteBuffers(1, &_buffer->buffer);
	}

	void* GL::mapBuffer(const Trident::Device* device, Trident::Buffer* buffer, uint64 offset, uint64 size) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		GL::Buffer* _buffer = static_cast<GL::Buffer*>(buffer);

		if (_buffer->mappedRange == nullptr) {
			glBindBuffer(_buffer->target, _buffer->buffer);

			_buffer->mappedRange = glMapBufferRange(_buffer->target, 0, _buffer->getSize(), _buffer->mapAccessFlags);

			ADAPTER_ASSERT(_buffer->mappedRange != nullptr, "Failed to map OpenGL buffer range.");
		}

		uint8* mappedRange = static_cast<uint8*>(_buffer->mappedRange) + offset;

		return static_cast<void*>(mappedRange);
	}

	void GL::unmapBuffer(const Trident::Device* device, Trident::Buffer* buffer) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		GL::Buffer* _buffer = static_cast<GL::Buffer*>(buffer);

		glBindBuffer(_buffer->target, _buffer->buffer);

		const GLboolean result = glUnmapBuffer(_buffer->target);

		ADAPTER_ASSERT(result == GL_TRUE, "Failed to unmap OpenGL buffer.");

		_buffer->mappedRange = nullptr;
	}
}