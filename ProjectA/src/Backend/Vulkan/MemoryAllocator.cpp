#include "pch.hpp"

#include "Backend/Vulkan/MemoryAllocator.hpp"

namespace ProjectA {
	const VulkanPhysicalDevice* MemoryAllocator::physicalDevice;
	std::vector<MemoryBlock> MemoryAllocator::memoryBlocks;
	std::vector<uint32> MemoryAllocator::currentBufferMemoryBlockIndices;
	std::vector<uint32> MemoryAllocator::currentImageMemoryBlockIndices;

	void MemoryAllocator::initialize(const VulkanPhysicalDevice& physicalDevice) {
		MemoryAllocator::physicalDevice = &physicalDevice;

		//ENGINE_TRACE("Buffer image granularity: []", physicalDevice.properties.limits.bufferImageGranularity);
		//ENGINE_TRACE("Max. memory allocation count: []", physicalDevice.properties.limits.maxMemoryAllocationCount);

		currentBufferMemoryBlockIndices.resize(physicalDevice.memoryProperties.memoryTypeCount);
		std::fill(currentBufferMemoryBlockIndices.begin(), currentBufferMemoryBlockIndices.end(), UNDEFINED_MEMORY_BLOCK_INDEX);

		currentImageMemoryBlockIndices.resize(physicalDevice.memoryProperties.memoryTypeCount);
		std::fill(currentImageMemoryBlockIndices.begin(), currentImageMemoryBlockIndices.end(), UNDEFINED_MEMORY_BLOCK_INDEX);

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
		const MemorySuballocation memorySuballocation = suballocate(device, memoryTypeIndex, size, currentBufferMemoryBlockIndices);

		ENGINE_DEBUG("Suballocating buffer resource inside memory block (offset = [], size = []).", memorySuballocation.offset, memorySuballocation.size);

		MemoryBlock& memoryBlock = memoryBlocks[memorySuballocation.blockIndex];

		const VkResult result = vkBindBufferMemory(device, buffer, memoryBlock.memory, memoryBlock.offset);

		ENGINE_ASSERT(result == VK_SUCCESS, "Could not bind memory for Vulkan buffer ([]).", string_VkResult(result));

		// Only advance the block offset once we are sure that the memory has been correctly bound to the buffer.
		memoryBlock.offset += size;
		memoryBlock.suballocationCount += 1;

		return memorySuballocation;
	}

	MemorySuballocation MemoryAllocator::suballocateImage(VkDevice device, VkImage image, uint32 memoryTypeIndex, VkDeviceSize size) {
		const MemorySuballocation memorySuballocation = suballocate(device, memoryTypeIndex, size, currentImageMemoryBlockIndices);

		ENGINE_DEBUG("Suballocating image resource inside memory block (offset = [], size = []).", memorySuballocation.offset, memorySuballocation.size);

		MemoryBlock& memoryBlock = memoryBlocks[memorySuballocation.blockIndex];

		const VkResult result = vkBindImageMemory(device, image, memoryBlock.memory, memoryBlock.offset);

		ENGINE_ASSERT(result == VK_SUCCESS, "Could not bind memory for Vulkan image ([]).", string_VkResult(result));

		// Only advance the block offset once we are sure that the memory has been correctly bound to the image.
		memoryBlock.offset += size;
		memoryBlock.suballocationCount += 1;

		return memorySuballocation;
	}

	void* MemoryAllocator::mapSuballocation(VkDevice device, const MemorySuballocation& memorySuballocation, VkDeviceSize size, VkDeviceSize offset) {
		MemoryBlock& memoryBlock = memoryBlocks[memorySuballocation.blockIndex];

		ENGINE_ASSERT(offset <= memoryBlock.offset, "The mapping offset is outside of the used range for the memory block.");
		ENGINE_ASSERT(size > 0, "The mapping size must not be 0.");
		ENGINE_ASSERT(offset + size <= memoryBlock.offset, "The offsetted mapping size is larger than the size of the allocated memory block.");

		if (memoryBlock.mappedSuballocationCount == 0) {
			// Map the whole block.
			const VkResult result = vkMapMemory(device, memoryBlock.memory, 0, memoryBlock.size, 0, &memoryBlock.mappedRange);

			ENGINE_ASSERT(result == VK_SUCCESS, "Could not map Vulkan memory. ([])", string_VkResult(result));
		}

		memoryBlock.mappedSuballocationCount += 1;

		return static_cast<void*>(static_cast<uint8*>(memoryBlock.mappedRange) + memorySuballocation.offset + offset);
	}

	void MemoryAllocator::unmapSuballocation(VkDevice device, uint32 memoryBlockIndex) {
		MemoryBlock& memoryBlock = memoryBlocks[memoryBlockIndex];

		if (memoryBlock.mappedSuballocationCount <= 1) {
			const VkMemoryType memoryType = physicalDevice->memoryProperties.memoryTypes[memoryBlock.memoryTypeIndex];

			if (!(memoryType.propertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)) {
				// TODO: Expose vkFlushMappedMemoryRanges.
				ENGINE_WARN("Non-coherent memory must be manually flushed.");

				/*VkMappedMemoryRange mappedMemoryRange{};
				mappedMemoryRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
				mappedMemoryRange.pNext = nullptr;
				mappedMemoryRange.memory = memoryBlock.memory;
				mappedMemoryRange.offset = 0;
				mappedMemoryRange.size = memoryBlock.offset;

				// Manually flush the mapped range.
				const VkResult result = vkFlushMappedMemoryRanges(device, 1, &mappedMemoryRange);

				ENGINE_ASSERT(result == VK_SUCCESS, "Could not flush mapped memory range ([]).", string_VkResult(result));*/
			}

			vkUnmapMemory(device, memoryBlock.memory);

			memoryBlock.mappedRange = nullptr;
			memoryBlock.mappedSuballocationCount = 0;

			return;
		}

		memoryBlock.mappedSuballocationCount -= 1;
	}

	void MemoryAllocator::freeSuballocation(VkDevice device, uint32 memoryBlockIndex) {
		MemoryBlock& memoryBlock = memoryBlocks[memoryBlockIndex];

		if (memoryBlock.suballocationCount <= 1) {
			ENGINE_DEBUG("Freeing memory block [].", memoryBlockIndex);

			vkFreeMemory(device, memoryBlock.memory, nullptr);

			memoryBlock.suballocationCount = 0;

			// Reset corresponding memory block index.
			for (uint32 i = 0; i < physicalDevice->memoryProperties.memoryTypeCount; i += 1) {
				if (currentBufferMemoryBlockIndices[i] == memoryBlockIndex) {
					currentBufferMemoryBlockIndices[i] = UNDEFINED_MEMORY_BLOCK_INDEX;
				}

				if (currentImageMemoryBlockIndices[i] == memoryBlockIndex) {
					currentImageMemoryBlockIndices[i] = UNDEFINED_MEMORY_BLOCK_INDEX;
				}
			}

			// TODO: Remove memory block from the list?

			return;
		}

		memoryBlock.suballocationCount -= 1;

		ENGINE_DEBUG("Freeing suballocation from memory block []. Suballocation count is now [].", memoryBlockIndex, memoryBlock.suballocationCount);
	}

	void MemoryAllocator::createAllocation(VkDevice device, uint32 memoryTypeIndex, VkDeviceSize size, std::vector<uint32>& currentMemoryBlockIndices) {
		const VkDeviceSize blockSize = std::max(size, DEFAULT_MEMORY_BLOCK_SIZE);

		const uint32 memoryBlockIndex = static_cast<uint32>(memoryBlocks.size());

		ENGINE_DEBUG(
			"Creating new memory allocation [] of type [] and size []Mb.",
			memoryBlockIndex,
			string_VkMemoryPropertyFlags(physicalDevice->memoryProperties.memoryTypes[memoryTypeIndex].propertyFlags),
			static_cast<float32>(blockSize) / 1000000.0f
		);

		VkMemoryAllocateInfo memoryAllocateInfo{};
		memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		memoryAllocateInfo.pNext = nullptr;
		memoryAllocateInfo.allocationSize =	blockSize;
		memoryAllocateInfo.memoryTypeIndex = memoryTypeIndex;

		MemoryBlock memoryBlock{};

		const VkResult result = vkAllocateMemory(device, &memoryAllocateInfo, nullptr, &memoryBlock.memory);

		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to allocate Vulkan memory ([]).", string_VkResult(result));

		memoryBlock.offset = 0;
		memoryBlock.size = blockSize;
		memoryBlock.memoryTypeIndex = memoryTypeIndex;
		memoryBlock.suballocationCount = 0;
		memoryBlock.mappedSuballocationCount = 0;

		memoryBlocks.emplace_back(memoryBlock);

		currentMemoryBlockIndices[memoryTypeIndex] = memoryBlockIndex;
	}

	MemorySuballocation MemoryAllocator::suballocate(VkDevice device, uint32 memoryTypeIndex, VkDeviceSize size, std::vector<uint32>& currentMemoryBlockIndices) {
		uint32 currentMemoryBlockIndex = currentMemoryBlockIndices[memoryTypeIndex];

		if (currentMemoryBlockIndex == UNDEFINED_MEMORY_BLOCK_INDEX) {
			createAllocation(device, memoryTypeIndex, size, currentMemoryBlockIndices);
		}

		currentMemoryBlockIndex = currentMemoryBlockIndices[memoryTypeIndex];
		MemoryBlock* currentMemoryBlock = &memoryBlocks[currentMemoryBlockIndex];

		// If the current memory block cannot fully hold this suballocation, close it and open a new block.
		if (currentMemoryBlock->size - currentMemoryBlock->offset < size) {
			ENGINE_DEBUG("Remaining block size is insufficient.");

			createAllocation(device, memoryTypeIndex, size, currentMemoryBlockIndices);

			currentMemoryBlockIndex = currentMemoryBlockIndices[memoryTypeIndex];
			currentMemoryBlock = &memoryBlocks[currentMemoryBlockIndex];
		}

		MemorySuballocation memorySuballocation{};
		memorySuballocation.blockIndex = currentMemoryBlockIndex;
		memorySuballocation.offset = currentMemoryBlock->offset;
		memorySuballocation.size = size;

		return memorySuballocation;
	}
}