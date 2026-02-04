// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/Descriptor/BufferDescriptor.hpp"
#include "Engine/Validation/Descriptor.hpp"

namespace Trident {
	class Buffer {
		public:
			virtual ~Buffer() = default;

			/// Returns the size in bytes of the buffer.
			uint64 getSize() const {
				return size;
			}

			/// Maps a range of the buffer to CPU memory.
			/// The offset and size must be provided in bytes.
			template<typename T>
			T* map(uint64 offset, uint64 size) const {
				return static_cast<T*>(getMappedRange(offset, size));
			}

			/// Unmaps the buffer.
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
			// NOTE: Only used for debug assertions.
			BufferUsage usage;

		protected:
			uint64 size;
	};
}