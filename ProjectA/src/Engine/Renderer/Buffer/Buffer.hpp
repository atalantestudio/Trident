#pragma once

#include "pch.hpp"

namespace ProjectA {
	struct BufferDescriptor {
		uint64 size;
		uint32 usage; // From BufferUsage enum.
		bool mapped;
	};

	class Buffer {
		public:
			virtual ~Buffer() = default;

			virtual void map(const uint64 offset, const uint64 size) = 0;

			virtual void write(const void* data, const uint64 offset, const uint64 size) = 0;

			virtual void unmap() = 0;
	};
}