// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/Base.hpp"

#include "Engine/API/Rendering/Access.hpp"
#include "Engine/API/Rendering/BufferUsage.hpp"
#include "Engine/API/Rendering/Descriptor/BufferDescriptor.hpp"

namespace Trident {
	class BaseBuffer {
		public:
			~BaseBuffer();

			/// Returns the size in bytes of the buffer.
			inline uint64 getSize() const {
				return size;
			}

			inline uint32 getStride() const {
				return stride;
			}

			/// Returns the buffer usage flags.
			inline BufferUsage getUsage() const {
				return usage;
			}

			/// Returns the current buffer access flags.
			inline Access getAccess() const {
				return access;
			}

			/// Maps a range of the buffer to host memory.
			/// The offset and size must be provided in bytes.
			void* map(uint64 offset, uint64 size) const;

			/// Unmaps the buffer.
			void unmap() const;

		protected:
			explicit BaseBuffer(const Device* device, const BufferDescriptor& descriptor, uint32 stride);

		// TODO: Make private.
		public:
			uint64 size;
			uint32 stride;
			BufferUsage usage;

			// TODO: Store memory suballocation.

			Access access = Access::NONE;

		private:
			const Device* device;
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