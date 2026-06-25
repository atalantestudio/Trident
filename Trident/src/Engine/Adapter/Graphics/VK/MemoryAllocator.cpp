// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/VK/Device.hpp"
#include "Engine/Adapter/Graphics/VK/MemoryAllocator.hpp"
#include "Engine/Adapter/Graphics/VK/PhysicalDevice.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace Trident {
	#if defined(BUILD_MODE_DEBUG)
		static sequence<char8> formatSize(uint64 size) {
			const float64 decimalSize = static_cast<float64>(size);

			if (size >= 1000000000000) {
				return Logger::format("[]Tb", decimalSize / 1000000000000.0);
			}

			if (size >= 1000000000) {
				return Logger::format("[]Gb", decimalSize / 1000000000.0);
			}

			if (size >= 1000000) {
				return Logger::format("[]Mb", decimalSize / 1000000.0);
			}

			const std::string formattedSize = toString(decimalSize);

			return sequence<char8>(&formattedSize[0], formattedSize.size());
		}
	#endif

	VK::MemoryAllocator::MemoryAllocator(const Trident::Device* device) :
		Trident::MemoryAllocator(device),
		physicalDevice(static_cast<const VK::PhysicalDevice*>(device->physicalDevice))
	{
		ADAPTER_DEBUG("Buffer image granularity: []", this->physicalDevice->properties.limits.bufferImageGranularity);
		ADAPTER_DEBUG("Max. memory allocation count: []", this->physicalDevice->properties.limits.maxMemoryAllocationCount);

		currentBufferMemoryAllocationIndices = sequence<uint32>(this->physicalDevice->memoryProperties.memoryTypeCount);
		std::fill(currentBufferMemoryAllocationIndices.begin(), currentBufferMemoryAllocationIndices.end(), UNDEFINED_MEMORY_ALLOCATION_INDEX);

		currentImageMemoryAllocationIndices = sequence<uint32>(this->physicalDevice->memoryProperties.memoryTypeCount);
		std::fill(currentImageMemoryAllocationIndices.begin(), currentImageMemoryAllocationIndices.end(), UNDEFINED_MEMORY_ALLOCATION_INDEX);

		#if defined(BUILD_MODE_DEBUG)
			#if defined(DEBUG_ENUMERATE_HEAPS)
				enumerateMemoryHeaps();
			#endif

			#if defined(DEBUG_ENUMERATE_TYPES)
				enumerateMemoryTypes();
			#endif
		#endif
	}

	#if defined(BUILD_MODE_DEBUG)
		void VK::MemoryAllocator::enumerateMemoryHeaps() {
			uint32 memoryHeapIndex = 0;

			while (memoryHeapIndex < physicalDevice->memoryProperties.memoryHeapCount) {
				const VkMemoryHeap& memoryHeap = physicalDevice->memoryProperties.memoryHeaps[memoryHeapIndex];

				ADAPTER_DEBUG(
					"Memory heap []:\n"
					"- Size: [] ([])\n"
					"- Flags: []",
					memoryHeapIndex,
					memoryHeap.size,
					formatSize(memoryHeap.size),
					string_VkMemoryHeapFlags(memoryHeap.flags)
				);

				memoryHeapIndex += 1;
			}
		}

		void VK::MemoryAllocator::enumerateMemoryTypes() {
			for (uint32 memoryTypeIndex = 0; memoryTypeIndex < physicalDevice->memoryProperties.memoryTypeCount; memoryTypeIndex += 1) {
				const VkMemoryType memoryType = physicalDevice->memoryProperties.memoryTypes[memoryTypeIndex];

				ADAPTER_DEBUG(
					"Memory type []:\n"
					"- Property flags: []\n"
					"- Heap: []",
					memoryTypeIndex,
					string_VkMemoryPropertyFlags(memoryType.propertyFlags),
					memoryType.heapIndex
				);
			}
		}
	#endif

	uint32 VK::MemoryAllocator::findSuitableMemoryType(VkMemoryPropertyFlags memoryPropertyFlags) {
		uint32 suitableMemoryTypeIndex = UNDEFINED_MEMORY_TYPE_INDEX;
		uint32 memoryTypeIndex = 0;

		while (memoryTypeIndex < physicalDevice->memoryProperties.memoryTypeCount) {
			const VkMemoryType memoryType = physicalDevice->memoryProperties.memoryTypes[memoryTypeIndex];
			const VkMemoryPropertyFlags matchingFlags = memoryType.propertyFlags & memoryPropertyFlags;

			if (matchingFlags > 0) {
				suitableMemoryTypeIndex = memoryTypeIndex;

				if (matchingFlags == memoryPropertyFlags) {
					break;
				}
			}

			memoryTypeIndex += 1;
		}

		return suitableMemoryTypeIndex;
	}

	uint32 VK::MemoryAllocator::findSuitableMemoryType(const VkMemoryRequirements& memoryRequirements, VkMemoryPropertyFlags memoryPropertyFlags) {
		uint32 suitableMemoryTypeIndex = UNDEFINED_MEMORY_TYPE_INDEX;
		uint32 memoryTypeIndex = 0;

		while (memoryTypeIndex < physicalDevice->memoryProperties.memoryTypeCount) {
			if (memoryRequirements.memoryTypeBits & BIT(memoryTypeIndex)) {
				const VkMemoryType memoryType = physicalDevice->memoryProperties.memoryTypes[memoryTypeIndex];
				const VkMemoryPropertyFlags matchingFlags = memoryType.propertyFlags & memoryPropertyFlags;

				if (matchingFlags > 0) {
					suitableMemoryTypeIndex = memoryTypeIndex;

					if (matchingFlags == memoryPropertyFlags) {
						break;
					}
				}
			}

			memoryTypeIndex += 1;
		}

		return suitableMemoryTypeIndex;
	}

	MemorySuballocation VK::MemoryAllocator::suballocateBuffer(VkBuffer buffer, uint32 memoryTypeIndex, const VkMemoryRequirements& memoryRequirements) {
		VkResult result;

		const VK::Device* device = static_cast<const VK::Device*>(this->device);

		bufferOrImageMemoryAllocationIndices = false;

		const uint32 allocationIndex = findAllocation(memoryTypeIndex, memoryRequirements, currentBufferMemoryAllocationIndices);
		const MemorySuballocation memorySuballocation = suballocate(allocationIndex, memoryRequirements.size, memoryRequirements.alignment);
		const MemoryAllocation& memoryAllocation = allocations[memorySuballocation.allocationIndex];

		ADAPTER_INFO("Creating new memory suballocation ([]) for buffer resource inside memory allocation [].", memoryAllocation.suballocationCount, memorySuballocation.allocationIndex);
		ADAPTER_DEBUG(
			"- Offset = []\n"
			"- Required size = []\n"
			"- Size = [] ([])\n"
			"- Alignment = []",
			memorySuballocation.offset,
			memoryRequirements.size,
			memorySuballocation.size,
			formatSize(memorySuballocation.size),
			memoryRequirements.alignment
		);

		result = device->vkBindBufferMemory(device->device, buffer, *pointer_cast<const VkDeviceMemory*>(&memoryAllocation.memory), memorySuballocation.offset);

		ADAPTER_ASSERT(result == VK_SUCCESS, "Could not bind memory for buffer ([]).", string_VkResult(result));

		return memorySuballocation;
	}

	MemorySuballocation VK::MemoryAllocator::suballocateImage(VkImage image, uint32 memoryTypeIndex, const VkMemoryRequirements& memoryRequirements) {
		VkResult result;

		const VK::Device* device = static_cast<const VK::Device*>(this->device);

		bufferOrImageMemoryAllocationIndices = true;

		const uint32 allocationIndex = findAllocation(memoryTypeIndex, memoryRequirements, currentImageMemoryAllocationIndices);
		const MemorySuballocation memorySuballocation = suballocate(allocationIndex, memoryRequirements.size, memoryRequirements.alignment);
		const MemoryAllocation& memoryAllocation = allocations[memorySuballocation.allocationIndex];

		ADAPTER_INFO("Created new memory suballocation ([]) for image resource inside memory allocation [].", memoryAllocation.suballocationCount, memorySuballocation.allocationIndex);
		ADAPTER_DEBUG(
			"- Offset = []\n"
			"- Required size = []\n"
			"- Size = [] ([])\n"
			"- Alignment = []",
			memorySuballocation.offset,
			memoryRequirements.size,
			memorySuballocation.size,
			formatSize(memorySuballocation.size),
			memoryRequirements.alignment
		);

		result = device->vkBindImageMemory(device->device, image, *pointer_cast<const VkDeviceMemory*>(&memoryAllocation.memory), memorySuballocation.offset);

		ADAPTER_ASSERT(result == VK_SUCCESS, "Could not bind memory for image ([]).", string_VkResult(result));

		return memorySuballocation;
	}

	void* VK::MemoryAllocator::mapSuballocation(const MemorySuballocation& memorySuballocation, VkDeviceSize offset, VkDeviceSize size) {
		const VK::Device* device = static_cast<const VK::Device*>(this->device);

		MemoryAllocation& memoryAllocation = allocations[memorySuballocation.allocationIndex];
		VkResult result;

		ADAPTER_ASSERT(offset <= memoryAllocation.usedOffset, "The mapping offset is outside of the used range for the memory allocation.");
		ADAPTER_ASSERT(offset + size <= memoryAllocation.usedOffset, "The offsetted mapping size ([]) is larger than the size of the memory allocation active range ([]).", offset + size, memoryAllocation.usedOffset);

		if (memoryAllocation.mappedSuballocationCount == 0) {
			// TODO: (Static allocations only) Map the active suballocation range and not the whole memory block.
			result = device->vkMapMemory(device->device, *pointer_cast<const VkDeviceMemory*>(&memoryAllocation.memory), 0, memoryAllocation.size, 0, &memoryAllocation.mappedRange);

			ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to map memory allocation. ([])", string_VkResult(result));

			ADAPTER_INFO(
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

	void VK::MemoryAllocator::unmapSuballocation(uint32 memoryAllocationIndex) {
		const VK::Device* device = static_cast<const VK::Device*>(this->device);

		MemoryAllocation& memoryAllocation = allocations[memoryAllocationIndex];

		if (memoryAllocation.mappedSuballocationCount <= 1) {
			const VkMemoryType memoryType = physicalDevice->memoryProperties.memoryTypes[memoryAllocation.memoryTypeIndex];

			if (!(memoryType.propertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)) {
				// TODO: Expose vkFlushMappedMemoryRanges to the graphics adapter.
				ADAPTER_WARNING("Non-coherent memory must be manually flushed.");

				/* VkMappedMemoryRange mappedMemoryRange{};
				mappedMemoryRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
				mappedMemoryRange.pNext = nullptr;
				mappedMemoryRange.memory = memoryAllocation.memory;
				mappedMemoryRange.offset = 0;
				mappedMemoryRange.size = memoryAllocation.offset;

				// Manually flush the mapped range.
				const VkResult result = device->vkFlushMappedMemoryRanges(device->device, 1, &mappedMemoryRange);

				ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to flush mapped memory allocation range ([]).", string_VkResult(result)); */
			}

			device->vkUnmapMemory(device->device, *pointer_cast<const VkDeviceMemory*>(&memoryAllocation.memory));

			ADAPTER_INFO("Unmapped memory allocation [].", memoryAllocationIndex);

			memoryAllocation.mappedRange = nullptr;
			memoryAllocation.mappedSuballocationCount = 0;

			return;
		}

		memoryAllocation.mappedSuballocationCount -= 1;
	}

	void VK::MemoryAllocator::freeSuballocation(uint32 memoryAllocationIndex) {
		const VK::Device* device = static_cast<const VK::Device*>(this->device);

		MemoryAllocation& memoryAllocation = allocations[memoryAllocationIndex];

		if (memoryAllocation.suballocationCount <= 1) {
			device->vkFreeMemory(device->device, *pointer_cast<const VkDeviceMemory*>(&memoryAllocation.memory), nullptr);

			memoryAllocation.suballocationCount = 0;

			uint32 memoryAllocationIndexIndex = 0;

			// Reset corresponding memory allocation index.
			while (memoryAllocationIndexIndex < currentBufferMemoryAllocationIndices.count()) {
				currentBufferMemoryAllocationIndices[memoryAllocationIndexIndex] = UNDEFINED_MEMORY_ALLOCATION_INDEX;
				currentImageMemoryAllocationIndices[memoryAllocationIndexIndex] = UNDEFINED_MEMORY_ALLOCATION_INDEX;

				memoryAllocationIndexIndex += 1;
			}

			ADAPTER_INFO("Freed memory allocation [].", memoryAllocationIndex);

			freeMemoryAllocationIndices.emplace_back(memoryAllocationIndex);

			return;
		}

		memoryAllocation.suballocationCount -= 1;

		ADAPTER_INFO("Freed suballocation from memory allocation []. The active suballocation count is now [].", memoryAllocationIndex, memoryAllocation.suballocationCount);
	}

	uint32 VK::MemoryAllocator::findAllocation(uint32 memoryTypeIndex, const VkMemoryRequirements& memoryRequirements, sequence<uint32>& currentMemoryAllocationIndices) {
		uint32 allocationIndex = currentMemoryAllocationIndices[memoryTypeIndex];

		if (allocationIndex == UNDEFINED_MEMORY_ALLOCATION_INDEX) {
			const VkDeviceSize alignedSize = max(memoryRequirements.size, memoryRequirements.alignment);

			// TODO
			// allocationIndex = allocateMemory(this, max(alignedSize, DYNAMIC_MEMORY_ALLOCATION_SIZE), nullptr);

			currentMemoryAllocationIndices[memoryTypeIndex] = allocationIndex;
		}

		return allocationIndex;
	}

	uint32 VK::allocateMemory(Trident::MemoryAllocator* memoryAllocator, uint64 size, MemoryProperty memoryProperties) {
		VK::MemoryAllocator* _memoryAllocator = static_cast<VK::MemoryAllocator*>(memoryAllocator);
		const VK::Device* _device = static_cast<const VK::Device*>(memoryAllocator->device);
		const uint32 memoryTypeIndex = _memoryAllocator->findSuitableMemoryType(static_cast<VkMemoryPropertyFlags>(memoryProperties));

		uint32 memoryAllocationIndex = static_cast<uint32>(memoryAllocator->allocations.size());

		if (_memoryAllocator->freeMemoryAllocationIndices.size() > 0) {
			memoryAllocationIndex = _memoryAllocator->freeMemoryAllocationIndices.back();

			_memoryAllocator->freeMemoryAllocationIndices.pop_back();
		}

		VkMemoryAllocateInfo memoryAllocateInfo{};
		memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		memoryAllocateInfo.pNext = nullptr;
		memoryAllocateInfo.allocationSize = size;
		memoryAllocateInfo.memoryTypeIndex = memoryTypeIndex;

		VkDeviceMemory memory;

		const VkResult result = _device->vkAllocateMemory(_device->device, &memoryAllocateInfo, nullptr, &memory);

		ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to allocate memory ([]).", string_VkResult(result));

		MemoryAllocation memoryAllocation{};
		memoryAllocation.memory = *pointer_cast<uint64*>(&memory);
		memoryAllocation.size = size;
		memoryAllocation.usedOffset = 0;
		memoryAllocation.memoryTypeIndex = memoryTypeIndex;
		memoryAllocation.suballocationCount = 0;
		memoryAllocation.lastSuballocationAlignment = 0;
		memoryAllocation.mappedSuballocationCount = 0;

		if (memoryAllocationIndex == memoryAllocator->allocations.size()) {
			memoryAllocator->allocations.emplace_back(memoryAllocation);
		}
		else {
			memoryAllocator->allocations[memoryAllocationIndex] = memoryAllocation;
		}

		return memoryAllocationIndex;
	}

	MemorySuballocation VK::suballocateMemory(Trident::MemoryAllocator* memoryAllocator, uint32 allocationIndex, uint64 size, uint64 alignment) {
		const VK::PhysicalDevice* physicalDevice = static_cast<const VK::PhysicalDevice*>(memoryAllocator->device->physicalDevice);
		VK::MemoryAllocator* _memoryAllocator = static_cast<VK::MemoryAllocator*>(memoryAllocator);
		MemoryAllocation* memoryAllocation = &memoryAllocator->allocations[allocationIndex];
		uint64 usedOffset = memoryAllocation->usedOffset;

		const VkDeviceSize alignedSize = max(size, alignment);
		const VkDeviceSize remainder = usedOffset % alignment;

		if (remainder > 0) {
			// Ensure that the offset meets the suballocation's alignment criteria.
			usedOffset += alignment - remainder;
		}

		// If the current memory allocation cannot fully hold this suballocation, close it and open a new one.
		// In some cases the aligned offset can be larger than the allocation's size.
		if (usedOffset > memoryAllocation->size || memoryAllocation->size - usedOffset < alignedSize) {
			ADAPTER_WARNING(
				"Remaining size in memory allocation [] ([]) is insufficient for suballocation with size [] and alignment [] (base size = []).",
				allocationIndex,
				memoryAllocation->size - usedOffset,
				alignedSize,
				alignment,
				size
			);

			const VkMemoryPropertyFlags memoryPropertyFlags = physicalDevice->memoryProperties.memoryTypes[memoryAllocation->memoryTypeIndex].propertyFlags;

			allocationIndex = allocateMemory(memoryAllocator, max(alignedSize, MemoryAllocator::DYNAMIC_MEMORY_ALLOCATION_SIZE), static_cast<MemoryProperty>(memoryPropertyFlags));

			if (_memoryAllocator->bufferOrImageMemoryAllocationIndices) {
				_memoryAllocator->currentImageMemoryAllocationIndices[memoryAllocation->memoryTypeIndex] = allocationIndex;
			}
			else {
				_memoryAllocator->currentBufferMemoryAllocationIndices[memoryAllocation->memoryTypeIndex] = allocationIndex;
			}

			memoryAllocation = &memoryAllocator->allocations[allocationIndex];
			usedOffset = 0;
		}

		MemorySuballocation memorySuballocation{};
		memorySuballocation.allocationIndex = allocationIndex;
		memorySuballocation.offset = usedOffset;
		memorySuballocation.size = alignedSize;

		memoryAllocation->usedOffset = memorySuballocation.offset + memorySuballocation.size;
		memoryAllocation->suballocationCount += 1;

		return memorySuballocation;
	}
}