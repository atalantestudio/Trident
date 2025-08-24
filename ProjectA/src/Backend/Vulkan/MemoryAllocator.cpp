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
			// enumerateMemoryHeaps();
			// enumerateMemoryTypes();
		#endif
	}

	void MemoryAllocator::destroy() {
		physicalDevice = nullptr;
	}

	#ifdef BUILD_MODE_DEBUG
		void MemoryAllocator::enumerateMemoryHeaps() {
			for (uint32 memoryHeapIndex = 0; memoryHeapIndex < physicalDevice->memoryProperties.memoryHeapCount; memoryHeapIndex += 1) {
				const VkMemoryHeap& memoryHeap = physicalDevice->memoryProperties.memoryHeaps[memoryHeapIndex];

				ENGINE_DEBUG(
					"Memory heap []:\n"
					"- Size: [] ([])\n"
					"- Flags: []",
					memoryHeapIndex,
					memoryHeap.size,
					formatSize(memoryHeap.size),
					string_VkMemoryHeapFlags(memoryHeap.flags)
				);
			}
		}

		void MemoryAllocator::enumerateMemoryTypes() {
			for (uint32 memoryTypeIndex = 0; memoryTypeIndex < physicalDevice->memoryProperties.memoryTypeCount; memoryTypeIndex += 1) {
				const VkMemoryType memoryType = physicalDevice->memoryProperties.memoryTypes[memoryTypeIndex];

				ENGINE_DEBUG(
					"Memory type []:\n"
					"- Property flags: []\n"
					"- Heap: []",
					memoryTypeIndex,
					string_VkMemoryPropertyFlags(memoryType.propertyFlags),
					memoryType.heapIndex
				);
			}
		}

		std::string MemoryAllocator::formatSize(uint64 size) {
			float64 decimalSize = static_cast<float64>(size);

			if (size >= 1000000000000) {
				// Tb.
				return Logger::format("[]Tb", decimalSize / 1000000000000.0);
			}

			if (size >= 1000000000) {
				// Gb.
				return Logger::format("[]Gb", decimalSize / 1000000000.0);
			}

			if (size >= 1000000) {
				// Mb.
				return Logger::format("[]Mb", decimalSize / 1000000.0);
			}

			return std::to_string(decimalSize);
		}
	#endif

	uint32 MemoryAllocator::findSuitableMemoryType(const VkMemoryRequirements& memoryRequirements, VkMemoryPropertyFlags memoryPropertyFlags) {
		uint32 suitableMemoryTypeIndex = UNDEFINED_MEMORY_TYPE_INDEX;

		for (uint32 memoryTypeIndex = 0; memoryTypeIndex < physicalDevice->memoryProperties.memoryTypeCount; memoryTypeIndex += 1) {
			if (!(memoryRequirements.memoryTypeBits & BIT(memoryTypeIndex))) {
				continue;
			}

			const VkMemoryType memoryType = physicalDevice->memoryProperties.memoryTypes[memoryTypeIndex];
			const VkMemoryHeap& memoryHeap = physicalDevice->memoryProperties.memoryHeaps[memoryType.heapIndex];

			if ((memoryType.propertyFlags & memoryPropertyFlags) == memoryPropertyFlags) {
				suitableMemoryTypeIndex = memoryTypeIndex;

				// TODO: Continue searching for better types.
				break;
			}
		}

		return suitableMemoryTypeIndex;
	}

	MemorySuballocation MemoryAllocator::suballocateBuffer(VkDevice device, VkBuffer buffer, uint32 memoryTypeIndex, const VkMemoryRequirements& memoryRequirements) {
		const MemorySuballocation memorySuballocation = suballocate(device, memoryTypeIndex, memoryRequirements, currentBufferMemoryAllocationIndices);
		MemoryAllocation& memoryAllocation = memoryAllocations[memorySuballocation.allocationIndex];

		ENGINE_INFO(
			"Creating new memory suballocation ([]) for buffer resource inside memory allocation [].\n"
			"	Offset = []\n"
			"	Required size = []\n"
			"	Size = [] ([])\n"
			"	Alignment = []",
			memoryAllocation.suballocationCount,
			memorySuballocation.allocationIndex,
			memorySuballocation.offset,
			memoryRequirements.size,
			memorySuballocation.size,
			formatSize(memorySuballocation.size),
			memoryRequirements.alignment
		);

		const VkResult result = vkBindBufferMemory(device, buffer, memoryAllocation.memory, memorySuballocation.offset);

		ENGINE_ASSERT(result == VK_SUCCESS, "Could not bind memory for Vulkan buffer ([]).", string_VkResult(result));

		memoryAllocation.offset = memorySuballocation.offset + memorySuballocation.size;
		memoryAllocation.suballocationCount += 1;

		return memorySuballocation;
	}

	MemorySuballocation MemoryAllocator::suballocateImage(VkDevice device, VkImage image, uint32 memoryTypeIndex, const VkMemoryRequirements& memoryRequirements) {
		const MemorySuballocation memorySuballocation = suballocate(device, memoryTypeIndex, memoryRequirements, currentImageMemoryAllocationIndices);
		MemoryAllocation& memoryAllocation = memoryAllocations[memorySuballocation.allocationIndex];

		ENGINE_INFO(
			"Creating new memory suballocation ([]) for image resource inside memory allocation [].\n"
			"	Offset = []\n"
			"	Required size = []\n"
			"	Size = [] ([])\n"
			"	Alignment = []",
			memoryAllocation.suballocationCount,
			memorySuballocation.allocationIndex,
			memorySuballocation.offset,
			memoryRequirements.size,
			memorySuballocation.size,
			formatSize(memorySuballocation.size),
			memoryRequirements.alignment
		);

		const VkResult result = vkBindImageMemory(device, image, memoryAllocation.memory, memorySuballocation.offset);

		ENGINE_ASSERT(result == VK_SUCCESS, "Could not bind memory for Vulkan image ([]).", string_VkResult(result));

		memoryAllocation.offset = memorySuballocation.offset + memorySuballocation.size;
		memoryAllocation.suballocationCount += 1;

		return memorySuballocation;
	}

	void* MemoryAllocator::mapSuballocation(VkDevice device, const MemorySuballocation& memorySuballocation, VkDeviceSize offset, VkDeviceSize size) {
		MemoryAllocation& memoryAllocation = memoryAllocations[memorySuballocation.allocationIndex];

		ENGINE_ASSERT(size > 0, "The mapping size must not be 0.");
		ENGINE_ASSERT(offset <= memoryAllocation.offset, "The mapping offset is outside of the used range for the memory allocation.");
		ENGINE_ASSERT(offset + size <= memoryAllocation.offset, "The offsetted mapping size is larger than the size of the memory allocation active range.");

		if (memoryAllocation.mappedSuballocationCount == 0) {
			// TODO: (Static allocations only) Map the active suballocation range and not the whole memory block.
			const VkResult result = vkMapMemory(device, memoryAllocation.memory, 0, memoryAllocation.size, 0, &memoryAllocation.mappedRange);

			ENGINE_ASSERT(result == VK_SUCCESS, "Could not map Vulkan memory. ([])", string_VkResult(result));

			ENGINE_INFO(
				"Mapped range ([], []) of memory allocation [].",
				0,
				memoryAllocation.size,
				memorySuballocation.allocationIndex
			);
		}

		memoryAllocation.mappedSuballocationCount += 1;

		uint8* mappedRange = static_cast<uint8*>(memoryAllocation.mappedRange) + memorySuballocation.offset + offset;

		return static_cast<void*>(mappedRange);
	}

	void MemoryAllocator::unmapSuballocation(VkDevice device, uint32 memoryAllocationIndex) {
		MemoryAllocation& memoryAllocation = memoryAllocations[memoryAllocationIndex];

		if (memoryAllocation.mappedSuballocationCount <= 1) {
			const VkMemoryType memoryType = physicalDevice->memoryProperties.memoryTypes[memoryAllocation.memoryTypeIndex];

			if (!(memoryType.propertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)) {
				// TODO: Expose vkFlushMappedMemoryRanges to the graphics adapter.
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

			ENGINE_INFO("Unmapped memory allocation [].", memoryAllocationIndex);

			memoryAllocation.mappedRange = nullptr;
			memoryAllocation.mappedSuballocationCount = 0;

			return;
		}

		memoryAllocation.mappedSuballocationCount -= 1;
	}

	void MemoryAllocator::freeSuballocation(VkDevice device, uint32 memoryAllocationIndex) {
		MemoryAllocation& memoryAllocation = memoryAllocations[memoryAllocationIndex];

		if (memoryAllocation.suballocationCount <= 1) {
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

			ENGINE_INFO("Freed memory allocation [].", memoryAllocationIndex);

			// TODO(review): Remove memory allocation from the list?
			return;
		}

		memoryAllocation.suballocationCount -= 1;

		ENGINE_INFO("Freed suballocation from memory allocation []. The active suballocation count is now [].", memoryAllocationIndex, memoryAllocation.suballocationCount);
	}

	uint32 MemoryAllocator::createAllocation(VkDevice device, uint32 memoryTypeIndex, VkDeviceSize size, std::vector<uint32>& currentMemoryAllocationIndices) {
		const VkDeviceSize allocationSize = std::max(size, DEFAULT_MEMORY_ALLOCATION_SIZE);

		const uint32 memoryAllocationIndex = static_cast<uint32>(memoryAllocations.size());

		ENGINE_INFO(
			"Creating new memory allocation ([]).\n"
			"	Property flags = []\n"
			"	Size = [] ([])",
			memoryAllocationIndex,
			string_VkMemoryPropertyFlags(physicalDevice->memoryProperties.memoryTypes[memoryTypeIndex].propertyFlags),
			allocationSize,
			formatSize(allocationSize)
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
		memoryAllocation.lastSuballocationAlignment = 0;
		memoryAllocation.mappedSuballocationCount = 0;

		memoryAllocations.emplace_back(memoryAllocation);

		currentMemoryAllocationIndices[memoryTypeIndex] = memoryAllocationIndex;

		return memoryAllocationIndex;
	}

	MemorySuballocation MemoryAllocator::suballocate(VkDevice device, uint32 memoryTypeIndex, const VkMemoryRequirements& memoryRequirements, std::vector<uint32>& currentMemoryAllocationIndices) {
		uint32 currentMemoryAllocationIndex = currentMemoryAllocationIndices[memoryTypeIndex];

		const VkDeviceSize size = std::max(memoryRequirements.size, memoryRequirements.alignment);

		if (currentMemoryAllocationIndex == UNDEFINED_MEMORY_ALLOCATION_INDEX) {
			currentMemoryAllocationIndex = createAllocation(device, memoryTypeIndex, size, currentMemoryAllocationIndices);
		}

		MemoryAllocation* currentMemoryAllocation = &memoryAllocations[currentMemoryAllocationIndex];

		VkDeviceSize offset = currentMemoryAllocation->offset;

		const VkDeviceSize remainder = offset % memoryRequirements.alignment;

		if (remainder != 0) {
			// Ensure that the offset meets the suballocation's alignment criteria.
			offset += memoryRequirements.alignment - remainder;
		}

		// If the current memory allocation cannot fully hold this suballocation, close it and open a new one.
		// In some cases the aligned offset can be larger than size of the allocation.
		if (offset > currentMemoryAllocation->size || currentMemoryAllocation->size - offset < size) {
			ENGINE_WARN(
				"Remaining size in memory allocation [] ([]) is insufficient for suballocation with size [] and alignment [] (base size = []).",
				currentMemoryAllocationIndex,
				currentMemoryAllocation->size - offset,
				size,
				memoryRequirements.alignment,
				memoryRequirements.size
			);

			currentMemoryAllocationIndex = createAllocation(device, memoryTypeIndex, size, currentMemoryAllocationIndices);
			currentMemoryAllocation = &memoryAllocations[currentMemoryAllocationIndex];

			offset = 0;
		}

		MemorySuballocation memorySuballocation{};
		memorySuballocation.allocationIndex = currentMemoryAllocationIndex;
		memorySuballocation.offset = offset;
		memorySuballocation.size = size;

		return memorySuballocation;
	}
}