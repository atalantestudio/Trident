#pragma once

#include "pch.hpp"

namespace ProjectA {
	struct BufferDescriptor {
		uint64 size;
		uint32 usage; // From BufferUsage enum.
	};

	class Buffer {
		public:
			virtual ~Buffer() = default;

			virtual void write(const void* data) const = 0;
	};
}