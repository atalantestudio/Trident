// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/GraphicsAdapter.hpp"
#include "Engine/API/Rendering/Access.hpp"
#include "Engine/API/Rendering/BufferUsage.hpp"
#include "Engine/API/Rendering/Descriptor/BufferDescriptor.hpp"
#include "Engine/Validation/Descriptor.hpp"

namespace Trident {
	class Buffer {
		public:
			~Buffer() {
				// adapter->destroyBuffer(this);
			}

			/// Returns the size in bytes of the buffer.
			uint64 getSize() const {
				return size;
			}

			/// Returns the current buffer usage flags.
			BufferUsage getUsage() const {
				return usage;
			}

			/// Returns the current buffer access flags.
			Access getAccess() const {
				return access;
			}

			void* map(uint64 offset, uint64 size) const {
				return adapter->mapBuffer(pointer_cast<const Buffer*>(this), offset, size);
			}

			/// Unmaps the buffer.
			void unmap() const {
				adapter->unmapBuffer(this);
			}

		protected:
			explicit Buffer(const GraphicsAdapter* adapter, const BufferDescriptor& descriptor) :
				adapter(adapter),
				size(descriptor.size),
				usage(descriptor.usage),
				access{}
			{
				// adapter->createBuffer(this, descriptor);
			}

		protected:
			uint64 size;
			// TODO: Add memory suballocation.
			BufferUsage usage;
			Access access;

		private:
			const GraphicsAdapter* adapter;
	};

	template<typename ElementType>
	class Buffer2 : public Buffer {
		public:
			/// Maps a range of the buffer to host memory.
			/// The offset and size must be provided in bytes.
			ElementType* map(uint64 offset, uint64 size) const {
				return static_cast<ElementType*>(Buffer::map(offset, size));
			}
	};
}