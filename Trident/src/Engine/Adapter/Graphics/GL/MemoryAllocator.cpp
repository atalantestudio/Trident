// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/GL/MemoryAllocator.hpp"
#include "Engine/Adapter/Graphics/GL/procedures.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace Trident {
	GL::MemoryAllocator::MemoryAllocator(const Trident::Device* device) :
		Trident::MemoryAllocator(device)
	{}

	uint32 GL::allocateMemory(Trident::MemoryAllocator* memoryAllocator, uint64 size, MemoryProperty memoryProperties) {
		const uint32 allocationIndex = static_cast<uint32>(memoryAllocator->allocations.size());

		MemoryAllocation memoryAllocation{};
		memoryAllocation.memoryTypeIndex = 0;
		memoryAllocation.size = size;
		memoryAllocation.usedOffset = 0;
		memoryAllocation.suballocationCount = 0;
		memoryAllocation.lastSuballocationAlignment = 0;
		memoryAllocation.mappedSuballocationCount = 0;
		memoryAllocation.mappedRange = nullptr;

		memoryAllocator->allocations.emplace_back(memoryAllocation);

		return allocationIndex;
	}

	MemorySuballocation GL::suballocateMemory(Trident::MemoryAllocator* memoryAllocator, uint32 allocationIndex, uint64 size, uint64 alignment) {
		MemoryAllocation* memoryAllocation = &memoryAllocator->allocations[allocationIndex];

		// TODO
		const uint64 usedOffset = 0;

		MemorySuballocation memorySuballocation{};
		memorySuballocation.allocationIndex = allocationIndex;
		memorySuballocation.offset = usedOffset;
		memorySuballocation.size = size;

		memoryAllocation->usedOffset = memorySuballocation.offset + memorySuballocation.size;
		memoryAllocation->suballocationCount += 1;

		return memorySuballocation;
	}
}