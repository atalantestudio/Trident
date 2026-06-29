// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Graphics/Device.hpp"
#include "Engine/API/Graphics/MemoryAllocator.hpp"

namespace Trident {
	MemoryAllocator::MemoryAllocator(const Device* device) :
		device(device)
	{}

	uint32 MemoryAllocator::allocate(bool dynamic, uint32 memoryTypeIndex, uint64 size, uint64 alignment) {
		#if defined(ENABLE_GRAPHICS_ADAPTER_VALIDATION_LAYER)
			uint32 errorCount = 0;

			if (size == 0) {
				ENGINE_ERROR("The allocation size must not be 0.");

				errorCount += 1;
			}

			if (alignment == 0) {
				ENGINE_ERROR("The allocation alignment must not be 0.");

				errorCount += 1;
			}

			const uint32 maxMemoryAllocationCount = device->getPhysicalDevice()->getLimits().maxMemoryAllocationCount;

			if (allocations.size() >= maxMemoryAllocationCount) {
				ENGINE_ERROR("There cannot be more memory allocations on this device.");

				errorCount += 1;
			}

			ENGINE_ASSERT(errorCount == 0, "Invalid argument(s) ([] errors).", errorCount);
		#endif

		const MemoryAllocation allocation = device->adapter->allocateMemoryImplementation(this, dynamic, memoryTypeIndex, size, alignment);

		uint32 allocationIndex;

		if (freeableAllocationIndices.size() > 0) {
			allocationIndex = freeableAllocationIndices.back();

			freeableAllocationIndices.pop_back();
		}
		else {
			allocationIndex = static_cast<uint32>(allocations.size());

			allocations.resize(allocations.size() + 1);
		}

		allocations[allocationIndex] = allocation;

		ENGINE_INFO("Created [] allocation ([]).", allocation.dynamic ? "dynamic" : "static", allocationIndex);
		ENGINE_DEBUG(
			"- Memory type = []\n"
			"- Size = []",
			allocation.memoryTypeIndex,
			allocation.size
		);

		#if defined(DEBUG_COUNT_ALLOCATED_MEMORY_SIZE)
			// TODO(graphics/memory-allocator): Also decrease when the allocation is freed.
			allocatedMemorySize += allocation.size;
		#endif

		return allocationIndex;
	}

	uint32 MemoryAllocator::findMemoryTypeIndex(MemoryProperty memoryProperties, bool transferDestination) const {
		return device->adapter->findMemoryTypeIndexImplementation(this, memoryProperties, transferDestination);
	}

	#if defined(DEBUG_COUNT_ALLOCATED_MEMORY_SIZE)
		uint64 MemoryAllocator::getAllocatedMemorySize() const {
			return allocatedMemorySize;
		}
	#endif

	MemoryRequirements MemoryAllocator::getMemoryRequirements(ref<BaseBuffer> buffer) const {
		return device->adapter->getBufferMemoryRequirementsImplementation(this, buffer);
	}

	MemoryRequirements MemoryAllocator::getMemoryRequirements(ref<Image> image) const {
		return device->adapter->getImageMemoryRequirementsImplementation(this, image);
	}

	MemorySuballocation MemoryAllocator::suballocate(uint32 allocationIndex, uint64 size, uint64 alignment) {
		#if defined(ENABLE_GRAPHICS_ADAPTER_VALIDATION_LAYER)
			uint32 errorCount = 0;

			if (allocationIndex < allocations.size()) {
				const MemoryAllocation& allocation = allocations[allocationIndex];
				const uint64 offset = min(allocation.size, ceil(allocation.usedOffset, alignment));

				if (allocation.size - offset < size) {
					ENGINE_ERROR("The suballocation requires [] bytes, but there are only [] bytes remaining in the allocation.", size, allocation.size - offset);

					errorCount += 1;
				}
			}
			else {
				ENGINE_ERROR("The allocation index is overflowing.");

				errorCount += 1;
			}

			ENGINE_ASSERT(errorCount == 0, "Invalid argument(s) ([] errors).", errorCount);
		#endif

		MemoryAllocation& allocation = allocations[allocationIndex];

		MemorySuballocation memorySuballocation{};
		memorySuballocation.allocationIndex = allocationIndex;
		memorySuballocation.offset = ceil(allocation.usedOffset, alignment);
		memorySuballocation.size = size;

		ENGINE_INFO("Created suballocation ([]) inside allocation [].", allocation.suballocationCount, memorySuballocation.allocationIndex);
		ENGINE_DEBUG(
			"- Offset = []\n"
			"- Size = []",
			memorySuballocation.offset,
			memorySuballocation.size
		);

		allocation.usedOffset = min(allocation.size, memorySuballocation.offset + memorySuballocation.size);
		allocation.suballocationCount += 1;

		return memorySuballocation;
	}

	void MemoryAllocator::suballocateStatic(uint32 allocationIndex, ref<BaseBuffer> buffer) {
		buffer->memorySuballocation = device->adapter->suballocateBufferMemoryImplementation(this, allocationIndex, buffer);
	}

	void MemoryAllocator::suballocateStatic(uint32 allocationIndex, ref<Image> image) {
		image->memorySuballocation = device->adapter->suballocateImageMemoryImplementation(this, allocationIndex, image);
	}

	void MemoryAllocator::suballocateDynamic(ref<BaseBuffer> buffer) {
		const MemoryRequirements memoryRequirements = getMemoryRequirements(buffer);
		const uint32 memoryTypeIndex = findMemoryTypeIndex(buffer->getMemorySuballocation().memoryProperties, buffer->getUsage() & BufferUsage::TRANSFER_DST);

		uint32 allocationIndex = findAllocationIndex(memoryTypeIndex, memoryRequirements);

		if (allocationIndex >= allocations.size()) {
			allocationIndex = allocate(true, memoryTypeIndex, max(MIN_DYNAMIC_MEMORY_ALLOCATION_SIZE, memoryRequirements.size), memoryRequirements.alignment);
		}

		suballocateStatic(allocationIndex, buffer);
	}

	void MemoryAllocator::suballocateDynamic(ref<Image> image) {
		const MemoryRequirements memoryRequirements = getMemoryRequirements(image);
		const uint32 memoryTypeIndex = findMemoryTypeIndex(image->getMemorySuballocation().memoryProperties, image->getUsage() & ImageUsage::TRANSFER_DST);

		uint32 allocationIndex = findAllocationIndex(memoryTypeIndex, memoryRequirements);

		if (allocationIndex >= allocations.size()) {
			allocationIndex = allocate(true, memoryTypeIndex, max(MIN_DYNAMIC_MEMORY_ALLOCATION_SIZE, memoryRequirements.size), memoryRequirements.alignment);
		}

		suballocateStatic(allocationIndex, image);
	}

	uint32 MemoryAllocator::findAllocationIndex(uint32 memoryTypeIndex, const MemoryRequirements& memoryRequirements) const {
		uint32 allocationIndex = 0;

		while (allocationIndex < allocations.size()) {
			const MemoryAllocation& allocation = allocations[allocationIndex];

			if (!allocation.dynamic) {
				allocationIndex += 1;

				continue;
			}

			if (allocation.memoryTypeIndex != memoryTypeIndex) {
				allocationIndex += 1;

				continue;
			}

			const uint64 offset = min(allocation.size, ceil(allocation.usedOffset, memoryRequirements.alignment));

			if (allocation.size - offset < memoryRequirements.size) {
				allocationIndex += 1;

				continue;
			}

			break;
		}

		return allocationIndex;
	}
}