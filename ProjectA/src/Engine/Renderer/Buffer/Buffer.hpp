#pragma once

#include "pch.hpp"

#include "Engine/Renderer/Buffer/BufferUsage.hpp"

namespace ProjectA {
	struct BufferDescriptor {
		// Size of the buffer in bytes.
		uint64 size;

		BufferUsage usage;
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
	};
}