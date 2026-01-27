// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/Descriptor/BufferDescriptor.hpp"
#include "Engine/Validation/Descriptor.hpp"

namespace Trident {
	class Buffer {
		public:
			virtual ~Buffer() = default;

			// Maps a range of the buffer to CPU memory.
			// `size` must be given in bytes. It is not multiplied by the size of T.
			template<typename T>
			T* map(uint64 offset, uint64 size) const {
				return static_cast<T*>(getMappedRange(offset, size));
			}

			// Unmaps the buffer.
			virtual void unmap() const = 0;

		protected:
			explicit Buffer(const BufferDescriptor& descriptor) :
				size(descriptor.size),
				usage(descriptor.usage)
			{
				DEBUG_DESCRIPTOR_GUARD(descriptor);
			}

		private:
			virtual void* getMappedRange(uint64 offset, uint64 size) const = 0;

		public:
			const uint64 size;

			// NOTE: This member is only used for assertions in the OpenGL adapter.
			const BufferUsage usage;
	};
}