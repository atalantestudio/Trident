#pragma once

#include "Engine/Renderer/Buffer/BufferUsage.hpp"

namespace ProjectA {
	struct BufferDescriptor {
		// Size of the buffer in bytes.
		uint64 size;

		BufferUsage usage;

		// Indicates if the buffer is going to be mapped for the application's lifetime.
		// Only used by OpenGL.
		bool persistent = false;
	};

	// TODO: Rename to GPUBuffer?
	// A memory blob on a GPU.
	class Buffer {
		public:
			virtual ~Buffer() = default;

			// Maps a buffer range to CPU memory.
			template<typename T>
			T* map(uint64 offset, uint64 size) const {
				return static_cast<T*>(getMappedRange(offset, size));
			}

			// Unmaps a previously mapped buffer range.
			virtual void unmap() const = 0;

		protected:
			Buffer(uint64 size) :
				size(size)
			{}

		private:
			virtual void* getMappedRange(uint64 offset, uint64 size) const = 0;

		public:
			const uint64 size;
	};
}