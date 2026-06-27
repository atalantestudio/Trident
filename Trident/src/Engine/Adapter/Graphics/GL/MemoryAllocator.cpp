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

	MemoryAllocation GL::allocateMemory(Trident::MemoryAllocator* memoryAllocator, bool dynamic, uint32 memoryTypeIndex, uint64 size, uint64 alignment) {
		MemoryAllocation memoryAllocation{};
		memoryAllocation.dynamic = dynamic;
		memoryAllocation.memory = 0;
		memoryAllocation.memoryTypeIndex = 0;
		memoryAllocation.size = size;
		memoryAllocation.usedOffset = 0;
		memoryAllocation.suballocationCount = 0;
		memoryAllocation.mappedSuballocationCount = 0;
		memoryAllocation.mappedRange = nullptr;

		return memoryAllocation;
	}

	uint32 GL::findMemoryTypeIndex(const Trident::MemoryAllocator* memoryAllocator, MemoryProperty memoryProperties) {
		return 0;
	}

	MemoryRequirements GL::getBufferMemoryRequirements(const Trident::MemoryAllocator* memoryAllocator, ref<Trident::BaseBuffer> buffer) {
		MemoryRequirements memoryRequirements{};
		memoryRequirements.size = buffer->getSize();
		memoryRequirements.alignment = GL_RESOURCE_ALIGNMENT;
		memoryRequirements.memoryTypeBits = 1;

		return memoryRequirements;
	}

	MemoryRequirements GL::getImageMemoryRequirements(const Trident::MemoryAllocator* memoryAllocator, ref<Trident::Image> image) {
		MemoryRequirements memoryRequirements{};
		memoryRequirements.size = image->getSize();
		memoryRequirements.alignment = GL_RESOURCE_ALIGNMENT;
		memoryRequirements.memoryTypeBits = 1;

		return memoryRequirements;
	}

	MemorySuballocation GL::suballocateBufferMemory(Trident::MemoryAllocator* memoryAllocator, uint32 allocationIndex, ref<Trident::BaseBuffer> buffer) {
		const MemoryRequirements memoryRequirements = getBufferMemoryRequirements(memoryAllocator, buffer);

		MemorySuballocation memorySuballocation = memoryAllocator->suballocate(allocationIndex, memoryRequirements.size, memoryRequirements.alignment);
		memorySuballocation.memoryProperties = buffer->getMemorySuballocation().memoryProperties;

		return memorySuballocation;
	}

	MemorySuballocation GL::suballocateImageMemory(Trident::MemoryAllocator* memoryAllocator, uint32 allocationIndex, ref<Trident::Image> image) {
		const MemoryRequirements memoryRequirements = getImageMemoryRequirements(memoryAllocator, image);

		MemorySuballocation memorySuballocation = memoryAllocator->suballocate(allocationIndex, memoryRequirements.size, memoryRequirements.alignment);
		memorySuballocation.memoryProperties = image->getMemorySuballocation().memoryProperties;

		return memorySuballocation;
	}
}