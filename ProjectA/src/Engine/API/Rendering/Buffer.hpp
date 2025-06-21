#pragma once

#include "Engine/API/Rendering/Descriptor/BufferDescriptor.hpp"

namespace ProjectA {
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
			explicit Buffer(uint64 size) :
				size(size)
			{}

		private:
			virtual void* getMappedRange(uint64 offset, uint64 size) const = 0;

		public:
			// The size in bytes of the buffer.
			const uint64 size;
	};
}