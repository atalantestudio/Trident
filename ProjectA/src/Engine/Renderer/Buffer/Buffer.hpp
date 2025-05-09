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

	class Buffer {
		public:
			virtual ~Buffer() = default;

			template<typename T>
			T* map(uint64 offset, uint64 size) const {
				return static_cast<T*>(getMappedRange(offset, size));
			}

			virtual void unmap() const = 0;

		private:
			virtual void* getMappedRange(uint64 offset, uint64 size) const = 0;

		protected:
			Buffer(uint64 size);

		public:
			const uint64 size;
	};
}