// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Graphics/MemoryAllocator.hpp"

namespace Trident {
	MemoryAllocator::MemoryAllocator(const Device* device) :
		device(device)
	{}

	uint32 MemoryAllocator::createAllocation(uint32 memoryTypeIndex, uint64 size) {
		return device->adapter->createMemoryAllocationImplementation(this, memoryTypeIndex, size);
	}
}