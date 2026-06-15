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

	uint32 GL::createMemoryAllocation(Trident::MemoryAllocator* memoryAllocator, uint32 memoryTypeIndex, uint64 size) {
		const uint32 allocationIndex = static_cast<uint32>(memoryAllocator->allocations.size());

		MemoryAllocation memoryAllocation{};
		memoryAllocation.memoryTypeIndex = memoryTypeIndex;
		memoryAllocation.offset = 0;
		memoryAllocation.size = size;
		memoryAllocation.suballocationCount = 0;
		memoryAllocation.lastSuballocationAlignment = 0;
		memoryAllocation.mappedSuballocationCount = 0;
		memoryAllocation.mappedRange = nullptr;

		memoryAllocator->allocations.emplace_back(memoryAllocation);

		return allocationIndex;
	}
}