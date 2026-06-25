// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Graphics/MemoryAllocator.hpp"

namespace Trident {
	MemoryAllocator::MemoryAllocator(const Device* device) :
		device(device)
	{}

	uint32 MemoryAllocator::allocate(uint64 size, MemoryProperty memoryProperties) {
		return device->adapter->allocateMemoryImplementation(this, size, memoryProperties);
	}

	MemorySuballocation MemoryAllocator::suballocate(uint32 allocationIndex, uint64 size, uint64 alignment) {
		return device->adapter->suballocateMemoryImplementation(this, allocationIndex, size, alignment);
	}
}