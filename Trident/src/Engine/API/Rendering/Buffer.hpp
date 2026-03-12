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
			uint64 getSize() const;

			/// Returns the current buffer usage flags.
			BufferUsage getUsage() const;

			/// Returns the current buffer access flags.
			Access getAccess() const;

			/// Maps a range of the buffer to host memory.
			/// The offset and size must be provided in bytes.
			void* map(uint64 offset, uint64 size) const;

			/// Unmaps the buffer.
			void unmap() const;

		protected:
			explicit BaseBuffer(const Device* device, const BufferDescriptor& descriptor);

		public:
			uint64 size;

			// TODO: Add memory suballocation.

			BufferUsage usage;
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