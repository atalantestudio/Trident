// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/Device.hpp"
#include "Engine/API/Graphics/MemoryAllocation.hpp"
#include "Engine/API/Graphics/MemorySuballocation.hpp"

namespace Trident {
	class MemoryAllocator {
		public:
			static constexpr uint64 DYNAMIC_MEMORY_ALLOCATION_SIZE = 16000000;

		public:
			/// Creates a memory allocation and returns its index.
			uint32 allocate(uint64 size, MemoryProperty memoryProperties);

			/// Suballocates a memory allocation and returns an object descripting the new partition.
			MemorySuballocation suballocate(uint32 allocationIndex, uint64 size, uint64 alignment);

		protected:
			explicit MemoryAllocator(const Device* device);

		// TODO: Don't expose.
		public:
			const Device* device;
			std::vector<MemoryAllocation> allocations;
	};
}