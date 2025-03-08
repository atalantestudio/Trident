#pragma once

#include "pch.hpp"

#include "Engine/Renderer/Buffer/BufferUsage.hpp"

namespace ProjectA {
	struct BufferDescriptor {
		uint64 size;
		BufferUsage usage;
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