#pragma once

#include "pch.hpp"

#include "Engine/Renderer/Buffer/BufferUsage.hpp"

namespace ProjectA {
	struct BufferDescriptor {
		uint64 size;
		BufferUsage usage;
	};

	class Buffer {
		public:
			virtual ~Buffer() = default;

			// TODO: Return the mapped range pointer?
			virtual void map(const uint64 offset, const uint64 size) = 0;

			// TODO: Remove to allow the client to use their own copy function?
			virtual void write(const void* data, const uint64 offset, const uint64 size) = 0;

			virtual void unmap() = 0;
	};
}