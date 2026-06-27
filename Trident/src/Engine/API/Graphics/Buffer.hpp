// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/Access.hpp"
#include "Engine/API/Graphics/Base.hpp"
#include "Engine/API/Graphics/BufferUsage.hpp"
#include "Engine/API/Graphics/BufferView.hpp"
#include "Engine/API/Graphics/Descriptor/BufferDescriptor.hpp"
#include "Engine/API/Graphics/MemorySuballocation.hpp"

namespace Trident {
	class BaseBuffer {
		public:
			~BaseBuffer();

			/// Returns the size in bytes of the buffer.
			uint64 getSize() const;

			/// Returns the buffer usage flags.
			BufferUsage getUsage() const;

			/// Returns the memory suballocation occupied by the buffer.
			MemorySuballocation getMemorySuballocation() const;

			/// Returns the current access flags for the buffer.
			Access getAccess() const;

			/// Unmaps the buffer.
			void unmap();

			template<typename T>
			ref<BufferView<T>> createView(uint64 offset, uint64 size);

			/// Maps a range of the buffer to host memory.
			/// The offset and size must be provided in bytes.
			void* map(uint64 offset, uint64 size);

		protected:
			explicit BaseBuffer(const Device* device, const BufferDescriptor& descriptor);

		private:
			const Device* device;

		// TODO: Make private.
		public:
			uint64 size;
			BufferUsage usage;
			MemorySuballocation memorySuballocation;
			Access access = Access::NONE;
	};

	// TODO(graphics/buffer): Deprecate (replace by BufferView).
	template<typename ElementType>
	class Buffer : public BaseBuffer {
		public:
			/// Maps a range of the buffer to host memory.
			/// The offset and size must be provided in bytes.
			inline ElementType* map(uint64 offset, uint64 size) {
				return static_cast<ElementType*>(BaseBuffer::map(offset, size));
			}
	};
}

#include "Engine/API/Graphics/Buffer.ipp"
#include "Engine/API/Graphics/BufferView.ipp"