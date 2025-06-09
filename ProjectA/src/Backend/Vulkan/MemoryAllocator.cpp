#include "pch.hpp"

#include "Backend/Vulkan/MemoryAllocator.hpp"

namespace ProjectA {
	const VulkanPhysicalDevice* MemoryAllocator::physicalDevice;
	std::vector<MemoryAllocation> MemoryAllocator::memoryAllocations;
	std::vector<uint32> MemoryAllocator::currentBufferMemoryAllocationIndices;
	std::vector<uint32> MemoryAllocator::currentImageMemoryAllocationIndices;

	void MemoryAllocator::initialize(const VulkanPhysicalDevice& physicalDevice) {
		MemoryAllocator::physicalDevice = &physicalDevice;

		//ENGINE_TRACE("Buffer image granularity: []", physicalDevice.properties.limits.bufferImageGranularity);
		//ENGINE_TRACE("Max. memory allocation count: []", physicalDevice.properties.limits.maxMemoryAllocationCount);

		currentBufferMemoryAllocationIndices.resize(physicalDevice.memoryProperties.memoryTypeCount);
		std::fill(currentBufferMemoryAllocationIndices.begin(), currentBufferMemoryAllocationIndices.end(), UNDEFINED_MEMORY_ALLOCATION_INDEX);

		currentImageMemoryAllocationIndices.resize(physicalDevice.memoryProperties.memoryTypeCount);
		std::fill(currentImageMemoryAllocationIndices.begin(), currentImageMemoryAllocationIndices.end(), UNDEFINED_MEMORY_ALLOCATION_INDEX);

		#ifdef BUILD_MODE_DEBUG
			enumerateMemoryTypes();
		#endif
	}

	void MemoryAllocator::destroy() {
		physicalDevice = nullptr;
	}

	#ifdef BUILD_MODE_DEBUG
		void MemoryAllocator::enumerateMemoryTypes() {
			for (uint32 memoryTypeIndex = 0; memoryTypeIndex < physicalDevice->memoryProperties.memoryTypeCount; memoryTypeIndex += 1) {
				const VkMemoryType memoryType = physicalDevice->memoryProperties.memoryTypes[memoryTypeIndex];
				const VkMemoryHeap& memoryHeap = physicalDevice->memoryProperties.memoryHeaps[memoryType.heapIndex];

				ENGINE_DEBUG(
					"Memory type []\n  Property flags: []\n  Heap index: []\n  Heap flags: []\n  Heap size: []",
					memoryTypeIndex,
					string_VkMemoryPropertyFlags(memoryType.propertyFlags),
					memoryType.heapIndex,
					string_VkMemoryHeapFlags(memoryHeap.flags),
					memoryHeap.size
				);
			}
		}
	#endif

	uint32 MemoryAllocator::findMemoryType(const VkMemoryRequirements& memoryRequirements, VkMemoryPropertyFlags memoryPropertyFlags) {
		ENGINE_DEBUG("Desired memory type: []", string_VkMemoryPropertyFlags(memoryPropertyFlags));

		uint32 suitableMemoryTypeIndex = UNDEFINED_MEMORY_TYPE_INDEX;

		for (uint32 memoryTypeIndex = 0; memoryTypeIndex < physicalDevice->memoryProperties.memoryTypeCount; memoryTypeIndex += 1) {
			if (!(memoryRequirements.memoryTypeBits & BIT(memoryTypeIndex))) {
				continue;
			}

			const VkMemoryType memoryType = physicalDevice->memoryProperties.memoryTypes[memoryTypeIndex];
			const VkMemoryHeap& memoryHeap = physicalDevice->memoryProperties.memoryHeaps[memoryType.heapIndex];

			if ((memoryType.propertyFlags & memoryPropertyFlags) == memoryPropertyFlags) {
				suitableMemoryTypeIndex = memoryTypeIndex;

				ENGINE_INFO("This memory type is suitable.");

				// TODO: Continue searching for better types?
				break;
			}
		}

		return suitableMemoryTypeIndex;
	}

	MemorySuballocation MemoryAllocator::suballocateBuffer(VkDevice device, VkBuffer buffer, uint32 memoryTypeIndex, VkDeviceSize size) {
		const MemorySuballocation memorySuballocation = suballocate(device, memoryTypeIndex, size, currentBufferMemoryAllocationIndices);

		ENGINE_DEBUG("Suballocating buffer resource inside memory allocation (offset = [], size = []).", memorySuballocation.offset, memorySuballocation.size);

		MemoryAllocation& memoryAllocation = memoryAllocations[memorySuballocation.allocationIndex];

		const VkResult result = vkBindBufferMemory(device, buffer, memoryAllocation.memory, memoryAllocation.offset);

		ENGINE_ASSERT(result == VK_SUCCESS, "Could not bind memory for Vulkan buffer ([]).", string_VkResult(result));

		// Only advance the allocation offset once we are sure that the memory has been correctly bound to the buffer.
		memoryAllocation.offset += size;
		memoryAllocation.suballocationCount += 1;

		return memorySuballocation;
	}

	MemorySuballocation MemoryAllocator::suballocateImage(VkDevice device, VkImage image, uint32 memoryTypeIndex, VkDeviceSize size) {
		const MemorySuballocation memorySuballocation = suballocate(device, memoryTypeIndex, size, currentImageMemoryAllocationIndices);

		ENGINE_DEBUG("Suballocating image resource inside memory allocation (offset = [], size = []).", memorySuballocation.offset, memorySuballocation.size);

		MemoryAllocation& memoryAllocation = memoryAllocations[memorySuballocation.allocationIndex];

		const VkResult result = vkBindImageMemory(device, image, memoryAllocation.memory, memoryAllocation.offset);

		ENGINE_ASSERT(result == VK_SUCCESS, "Could not bind memory for Vulkan image ([]).", string_VkResult(result));

		// Only advance the allocation offset once we are sure that the memory has been correctly bound to the image.
		memoryAllocation.offset += size;
		memoryAllocation.suballocationCount += 1;

		return memorySuballocation;
	}

	void* MemoryAllocator::mapSuballocation(VkDevice device, const MemorySuballocation& memorySuballocation, VkDeviceSize size, VkDeviceSize offset) {
		MemoryAllocation& memoryAllocation = memoryAllocations[memorySuballocation.allocationIndex];

		ENGINE_ASSERT(offset <= memoryAllocation.offset, "The mapping offset is outside of the used range for the memory allocation.");
		ENGINE_ASSERT(size > 0, "The mapping size must not be 0.");
		ENGINE_ASSERT(offset + size <= memoryAllocation.offset, "The offsetted mapping size is larger than the size of the allocated memory allocation.");

		if (memoryAllocation.mappedSuballocationCount == 0) {
			// Map the whole allocation.
			const VkResult result = vkMapMemory(device, memoryAllocation.memory, 0, memoryAllocation.size, 0, &memoryAllocation.mappedRange);

			ENGINE_ASSERT(result == VK_SUCCESS, "Could not map Vulkan memory. ([])", string_VkResult(result));
		}

		memoryAllocation.mappedSuballocationCount += 1;

		return static_cast<void*>(static_cast<uint8*>(memoryAllocation.mappedRange) + memorySuballocation.offset + offset);
	}

	void MemoryAllocator::unmapSuballocation(VkDevice device, uint32 memoryAllocationIndex) {
		MemoryAllocation& memoryAllocation = memoryAllocations[memoryAllocationIndex];

		if (memoryAllocation.mappedSuballocationCount <= 1) {
			const VkMemoryType memoryType = physicalDevice->memoryProperties.memoryTypes[memoryAllocation.memoryTypeIndex];

			if (!(memoryType.propertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)) {
				// TODO: Expose vkFlushMappedMemoryRanges.
				ENGINE_WARN("Non-coherent memory must be manually flushed.");

				/*VkMappedMemoryRange mappedMemoryRange{};
				mappedMemoryRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
				mappedMemoryRange.pNext = nullptr;
				mappedMemoryRange.memory = memoryAllocation.memory;
				mappedMemoryRange.offset = 0;
				mappedMemoryRange.size = memoryAllocation.offset;

				// Manually flush the mapped range.
				const VkResult result = vkFlushMappedMemoryRanges(device, 1, &mappedMemoryRange);

				ENGINE_ASSERT(result == VK_SUCCESS, "Could not flush mapped memory range ([]).", string_VkResult(result));*/
			}

			vkUnmapMemory(device, memoryAllocation.memory);

			memoryAllocation.mappedRange = nullptr;
			memoryAllocation.mappedSuballocationCount = 0;

			return;
		}

		memoryAllocation.mappedSuballocationCount -= 1;
	}

	void MemoryAllocator::freeSuballocation(VkDevice device, uint32 memoryAllocationIndex) {
		MemoryAllocation& memoryAllocation = memoryAllocations[memoryAllocationIndex];

		if (memoryAllocation.suballocationCount <= 1) {
			ENGINE_DEBUG("Freeing memory allocation [].", memoryAllocationIndex);

			vkFreeMemory(device, memoryAllocation.memory, nullptr);

			memoryAllocation.suballocationCount = 0;

			// Reset corresponding memory allocation index.
			for (uint32 i = 0; i < physicalDevice->memoryProperties.memoryTypeCount; i += 1) {
				if (currentBufferMemoryAllocationIndices[i] == memoryAllocationIndex) {
					currentBufferMemoryAllocationIndices[i] = UNDEFINED_MEMORY_ALLOCATION_INDEX;
				}

				if (currentImageMemoryAllocationIndices[i] == memoryAllocationIndex) {
					currentImageMemoryAllocationIndices[i] = UNDEFINED_MEMORY_ALLOCATION_INDEX;
				}
			}

			// TODO: Remove memory allocation from the list?

			return;
		}

		memoryAllocation.suballocationCount -= 1;

		ENGINE_DEBUG("Freeing suballocation from memory allocation []. Suballocation count is now [].", memoryAllocationIndex, memoryAllocation.suballocationCount);
	}

	void MemoryAllocator::createAllocation(VkDevice device, uint32 memoryTypeIndex, VkDeviceSize size, std::vector<uint32>& currentMemoryAllocationIndices) {
		const VkDeviceSize allocationSize = std::max(size, DEFAULT_MEMORY_ALLOCATION_SIZE);

		const uint32 memoryAllocationIndex = static_cast<uint32>(memoryAllocations.size());

		ENGINE_DEBUG(
			"Creating new memory allocation [] of type [] and size []Mb.",
			memoryAllocationIndex,
			string_VkMemoryPropertyFlags(physicalDevice->memoryProperties.memoryTypes[memoryTypeIndex].propertyFlags),
			static_cast<float32>(allocationSize) / 1000000.0f
		);

		VkMemoryAllocateInfo memoryAllocateInfo{};
		memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		memoryAllocateInfo.pNext = nullptr;
		memoryAllocateInfo.allocationSize =	allocationSize;
		memoryAllocateInfo.memoryTypeIndex = memoryTypeIndex;

		MemoryAllocation memoryAllocation{};

		const VkResult result = vkAllocateMemory(device, &memoryAllocateInfo, nullptr, &memoryAllocation.memory);

		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to allocate Vulkan memory ([]).", string_VkResult(result));

		memoryAllocation.offset = 0;
		memoryAllocation.size = allocationSize;
		memoryAllocation.memoryTypeIndex = memoryTypeIndex;
		memoryAllocation.suballocationCount = 0;
		memoryAllocation.mappedSuballocationCount = 0;

		memoryAllocations.emplace_back(memoryAllocation);

		currentMemoryAllocationIndices[memoryTypeIndex] = memoryAllocationIndex;
	}

	MemorySuballocation MemoryAllocator::suballocate(VkDevice device, uint32 memoryTypeIndex, VkDeviceSize size, std::vector<uint32>& currentMemoryAllocationIndices) {
		uint32 currentMemoryAllocationIndex = currentMemoryAllocationIndices[memoryTypeIndex];

		if (currentMemoryAllocationIndex == UNDEFINED_MEMORY_ALLOCATION_INDEX) {
			createAllocation(device, memoryTypeIndex, size, currentMemoryAllocationIndices);
		}

		currentMemoryAllocationIndex = currentMemoryAllocationIndices[memoryTypeIndex];
		MemoryAllocation* currentMemoryAllocation = &memoryAllocations[currentMemoryAllocationIndex];

		// If the current memory allocation cannot fully hold this suballocation, close it and open a new one.
		if (currentMemoryAllocation->size - currentMemoryAllocation->offset < size) {
			ENGINE_DEBUG("Remaining allocation size is insufficient.");

			createAllocation(device, memoryTypeIndex, size, currentMemoryAllocationIndices);

			currentMemoryAllocationIndex = currentMemoryAllocationIndices[memoryTypeIndex];
			currentMemoryAllocation = &memoryAllocations[currentMemoryAllocationIndex];
		}

		MemorySuballocation memorySuballocation{};
		memorySuballocation.allocationIndex = currentMemoryAllocationIndex;
		memorySuballocation.offset = currentMemoryAllocation->offset;
		memorySuballocation.size = size;

		return memorySuballocation;
	}
}