#pragma once

#include "pch.hpp"

#include "Engine/Renderer/Buffer/BufferUsage.hpp"

namespace ProjectA {
	struct BufferDescriptor {
		// Size of the buffer, in bytes.
		uint64 size;

		BufferUsage usage;
	};

	typedef void* BufferMappedRange;

	class Buffer {
		public:
			virtual ~Buffer() = default;

			// TODO: Return the mapped range pointer?
			virtual BufferMappedRange map(uint64 offset, uint64 size) const = 0;

			// TODO: Remove to allow the client to use their own copy function?
			//virtual void write(void* data, uint64 offset, uint64 size) = 0;

			virtual void unmap() const = 0;

			// Copies the contents of the current buffer to the provided destination buffer.
			virtual void copy(const ref<Buffer>& destinationBuffer, uint64 sourceOffset, uint64 destinationOffset, uint64 size) const = 0;
	};
}