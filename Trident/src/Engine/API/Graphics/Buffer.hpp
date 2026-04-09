// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/Base.hpp"
#include "Engine/API/Graphics/Access.hpp"
#include "Engine/API/Graphics/BufferUsage.hpp"
#include "Engine/API/Graphics/Descriptor/BufferDescriptor.hpp"

namespace Trident {
	class BaseBuffer {
		public:
			~BaseBuffer();

			/// Returns the size in bytes of the buffer.
			uint64 getSize() const;

			uint32 getStride() const;

			/// Returns the buffer usage flags.
			BufferUsage getUsage() const;

			/// Returns the current access flags for the buffer.
			Access getAccess() const;

			/// Unmaps the buffer.
			void unmap() const;

		protected:
			explicit BaseBuffer(const Device* device, const BufferDescriptor& descriptor, uint32 stride);

			/// Maps a range of the buffer to host memory.
			/// The offset and size must be provided in bytes.
			void* map(uint64 offset, uint64 size) const;

		private:
			const Device* device;

		// TODO: Make private.
		public:
			uint64 size;
			uint32 stride;
			BufferUsage usage;

			// TODO: Store memory suballocation.

			Access access = Access::NONE;
	};

	template<typename ElementType>
	class Buffer : public BaseBuffer {
		public:
			/// Maps a range of the buffer to host memory.
			/// The offset and size must be provided in bytes.
			inline ElementType* map(uint64 offset, uint64 size) const {
				return static_cast<ElementType*>(BaseBuffer::map(offset, size));
			}
	};
}