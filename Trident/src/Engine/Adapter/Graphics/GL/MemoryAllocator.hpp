// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/Device.hpp"
#include "Engine/API/Graphics/MemoryAllocator.hpp"

namespace Trident {
	class GL::MemoryAllocator : public Trident::MemoryAllocator {
		public:
			explicit MemoryAllocator(const Trident::Device* device);
	};

	namespace GL {
		uint32 allocateMemory(Trident::MemoryAllocator* memoryAllocator, uint64 size, MemoryProperty memoryProperties);

		MemorySuballocation suballocateMemory(Trident::MemoryAllocator* memoryAllocator, uint32 allocationIndex, uint64 size, uint64 alignment);
	}
}