// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/VK/Buffer.hpp"
#include "Engine/Adapter/Graphics/VK/Device.hpp"
#include "Engine/Adapter/Graphics/VK/Image.hpp"
#include "Engine/Adapter/Graphics/VK/MemoryAllocator.hpp"
#include "Engine/Adapter/Graphics/VK/PhysicalDevice.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace Trident {
	VK::MemoryAllocator::MemoryAllocator(const Trident::Device* device) :
		Trident::MemoryAllocator(device)
	{
		const ref<VK::PhysicalDevice> _physicalDevice = ref_cast<VK::PhysicalDevice>(device->physicalDevice);

		#if defined(DEBUG_ENUMERATE_MEMORY_HEAPS)
			enumerateMemoryHeaps();
		#endif

		#if defined(DEBUG_ENUMERATE_MEMORY_TYPES)
			enumerateMemoryTypes();
		#endif
	}

	#if defined(DEBUG_ENUMERATE_MEMORY_HEAPS)
		void VK::MemoryAllocator::enumerateMemoryHeaps() {
			uint32 memoryHeapIndex = 0;

			while (memoryHeapIndex < physicalDevice->memoryProperties.memoryHeapCount) {
				const VkMemoryHeap& memoryHeap = physicalDevice->memoryProperties.memoryHeaps[memoryHeapIndex];

				ADAPTER_DEBUG(
					"Memory heap []:\n"
					"- Size: []\n"
					"- Flags: []",
					memoryHeapIndex,
					memoryHeap.size,
					string_VkMemoryHeapFlags(memoryHeap.flags)
				);

				memoryHeapIndex += 1;
			}
		}
	#endif

	#if defined(DEBUG_ENUMERATE_MEMORY_TYPES)
		void VK::MemoryAllocator::enumerateMemoryTypes() {
			uint32 memoryTypeIndex = 0;

			while (memoryTypeIndex < physicalDevice->memoryProperties.memoryTypeCount) {
				const VkMemoryType memoryType = physicalDevice->memoryProperties.memoryTypes[memoryTypeIndex];

				ADAPTER_DEBUG(
					"Memory type []:\n"
					"- Property flags: []\n"
					"- Heap: []",
					memoryTypeIndex,
					string_VkMemoryPropertyFlags(memoryType.propertyFlags),
					memoryType.heapIndex
				);

				memoryTypeIndex += 1;
			}
		}
	#endif

	uint32 VK::MemoryAllocator::findMemoryTypeIndex(uint32 memoryTypeBits, VkMemoryPropertyFlags memoryPropertyFlags) {
		const ref<VK::PhysicalDevice> _physicalDevice = ref_cast<VK::PhysicalDevice>(device->physicalDevice);

		uint32 bestMemoryTypeIndex = 0;
		uint32 maxMemoryTypePropertyMatchingFlagCount = 0;
		uint32 memoryTypeIndex = 0;

		while (memoryTypeIndex < _physicalDevice->memoryProperties.memoryTypeCount) {
			if (memoryTypeBits & BIT(memoryTypeIndex)) {
				const VkMemoryType memoryType = _physicalDevice->memoryProperties.memoryTypes[memoryTypeIndex];
				const VkMemoryPropertyFlags matchingFlags = memoryType.propertyFlags & memoryPropertyFlags;
				const uint32 matchingFlagCount = countSetBits(matchingFlags);

				if (matchingFlagCount > maxMemoryTypePropertyMatchingFlagCount) {
					maxMemoryTypePropertyMatchingFlagCount = matchingFlagCount;
					bestMemoryTypeIndex = memoryTypeIndex;

					if (matchingFlags == memoryPropertyFlags) {
						break;
					}
				}
			}

			memoryTypeIndex += 1;
		}

		return bestMemoryTypeIndex;
	}

	void* VK::MemoryAllocator::mapSuballocation(const MemorySuballocation& memorySuballocation, VkDeviceSize offset, VkDeviceSize size) {
		const VK::Device* _device = static_cast<const VK::Device*>(device);

		MemoryAllocation& memoryAllocation = allocations[memorySuballocation.allocationIndex];

		ADAPTER_ASSERT(offset <= memoryAllocation.usedOffset, "The mapping offset is outside of the used range for the memory allocation.");
		ADAPTER_ASSERT(offset + size <= memoryAllocation.usedOffset, "The offsetted mapping size ([]) is larger than the size of the memory allocation active range ([]).", offset + size, memoryAllocation.usedOffset);

		if (memoryAllocation.mappedSuballocationCount == 0) {
			const VkResult result = _device->vkMapMemory(_device->device, *pointer_cast<const VkDeviceMemory*>(&memoryAllocation.memory), 0, memoryAllocation.size, 0, &memoryAllocation.mappedRange);

			ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to map memory allocation. ([])", string_VkResult(result));

			ADAPTER_INFO(
				"Mapped range ([], []) of memory allocation [].",
				0,
				memoryAllocation.size,
				memorySuballocation.allocationIndex
			);
		}

		uint8* mappedRange = static_cast<uint8*>(memoryAllocation.mappedRange) + memorySuballocation.offset + offset;

		return static_cast<void*>(mappedRange);
	}

	void VK::MemoryAllocator::unmapSuballocation(uint32 memoryAllocationIndex) {
		const VK::Device* _device = static_cast<const VK::Device*>(device);
		const ref<VK::PhysicalDevice> _physicalDevice = ref_cast<VK::PhysicalDevice>(device->physicalDevice);

		MemoryAllocation& memoryAllocation = allocations[memoryAllocationIndex];

		if (memoryAllocation.mappedSuballocationCount == 1) {
			const VkMemoryType memoryType = _physicalDevice->memoryProperties.memoryTypes[memoryAllocation.memoryTypeIndex];

			if (!(memoryType.propertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)) {
				ADAPTER_WARNING("Non-coherent memory must be manually flushed.");

				// TODO(graphics/vk/memory-allocator/flush-mapped-ranges)
				ENGINE_ERROR_UNIMPLEMENTED();

				/* VkMappedMemoryRange mappedMemoryRange{};
				mappedMemoryRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
				mappedMemoryRange.pNext = nullptr;
				mappedMemoryRange.memory = memoryAllocation.memory;
				mappedMemoryRange.offset = 0;
				mappedMemoryRange.size = memoryAllocation.offset;

				const VkResult result = _device->vkFlushMappedMemoryRanges(_device->device, 1, &mappedMemoryRange);

				ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to flush mapped memory allocation range ([]).", string_VkResult(result)); */
			}

			_device->vkUnmapMemory(_device->device, *pointer_cast<const VkDeviceMemory*>(&memoryAllocation.memory));

			ADAPTER_INFO("Unmapped memory allocation [].", memoryAllocationIndex);

			memoryAllocation.mappedRange = nullptr;
		}
	}

	void VK::MemoryAllocator::freeSuballocation(uint32 memoryAllocationIndex) {
		const VK::Device* _device = static_cast<const VK::Device*>(device);

		MemoryAllocation& memoryAllocation = allocations[memoryAllocationIndex];

		if (memoryAllocation.suballocationCount <= 1) {
			_device->vkFreeMemory(_device->device, *pointer_cast<const VkDeviceMemory*>(&memoryAllocation.memory), nullptr);

			memoryAllocation.suballocationCount = 0;

			ADAPTER_INFO("Freed memory allocation [].", memoryAllocationIndex);

			freeableAllocationIndices.emplace_back(memoryAllocationIndex);

			return;
		}

		memoryAllocation.suballocationCount -= 1;

		ADAPTER_INFO("Freed suballocation from memory allocation []. The active suballocation count is now [].", memoryAllocationIndex, memoryAllocation.suballocationCount);
	}

	MemoryAllocation VK::allocateMemory(Trident::MemoryAllocator* memoryAllocator, bool dynamic, uint32 memoryTypeIndex, uint64 size, uint64 alignment, bool shaderDeviceAddress) {
		const VK::Device* _device = static_cast<const VK::Device*>(memoryAllocator->device);
		const ref<VK::PhysicalDevice> _physicalDevice = ref_cast<VK::PhysicalDevice>(memoryAllocator->device->physicalDevice);
		const VkMemoryType memoryType = _physicalDevice->memoryProperties.memoryTypes[memoryTypeIndex];
		const VkMemoryHeap memoryHeap = _physicalDevice->memoryProperties.memoryHeaps[memoryType.heapIndex];

		ADAPTER_ASSERT(size < memoryHeap.size, "The allocation size must be smaller than the memory heap's size.");

		VkMemoryAllocateInfo memoryAllocateInfo{};
		memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		memoryAllocateInfo.pNext = nullptr;
		memoryAllocateInfo.allocationSize = size;
		memoryAllocateInfo.memoryTypeIndex = memoryTypeIndex;

		VkMemoryAllocateFlagsInfo memoryAllocateFlagsInfo;

		if (shaderDeviceAddress) {
			memoryAllocateFlagsInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO;
			memoryAllocateFlagsInfo.pNext = nullptr;
			memoryAllocateFlagsInfo.flags = VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT;
			memoryAllocateFlagsInfo.deviceMask = 0;

			memoryAllocateInfo.pNext = &memoryAllocateFlagsInfo;
		}

		VkDeviceMemory memory;

		const VkResult result = _device->vkAllocateMemory(_device->device, &memoryAllocateInfo, nullptr, &memory);

		ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to allocate memory ([]).", string_VkResult(result));

		MemoryAllocation memoryAllocation{};
		memoryAllocation.dynamic = dynamic;
		memoryAllocation.memory = *pointer_cast<uint64*>(&memory);
		memoryAllocation.memoryTypeIndex = memoryTypeIndex;
		memoryAllocation.size = size;
		memoryAllocation.usedOffset = 0;
		memoryAllocation.suballocationCount = 0;
		memoryAllocation.mappedSuballocationCount = 0;
		memoryAllocation.mappedRange = nullptr;

		return memoryAllocation;
	}

	uint32 VK::findMemoryTypeIndex(const Trident::MemoryAllocator* memoryAllocator, MemoryProperty memoryProperties, bool transferDestination) {
		const ref<VK::PhysicalDevice> physicalDevice = ref_cast<VK::PhysicalDevice>(memoryAllocator->device->physicalDevice);
		const VkMemoryPropertyFlags memoryPropertyFlags = static_cast<VkMemoryPropertyFlags>(memoryProperties);

		uint32 bestMemoryTypeIndex = 0;
		uint32 maxMemoryTypePropertyMatchingFlagCount = 0;
		uint32 memoryTypeIndex = 0;

		while (memoryTypeIndex < physicalDevice->memoryProperties.memoryTypeCount) {
			const VkMemoryType memoryType = physicalDevice->memoryProperties.memoryTypes[memoryTypeIndex];
			const VkMemoryPropertyFlags matchingFlags = memoryType.propertyFlags & memoryPropertyFlags;
			const uint32 matchingFlagCount = countSetBits(matchingFlags);

			if (matchingFlagCount > maxMemoryTypePropertyMatchingFlagCount) {
				maxMemoryTypePropertyMatchingFlagCount = matchingFlagCount;
				bestMemoryTypeIndex = memoryTypeIndex;

				if (matchingFlags == memoryPropertyFlags) {
					break;
				}
			}

			memoryTypeIndex += 1;
		}

		return bestMemoryTypeIndex;
	}

	MemoryRequirements VK::getBufferMemoryRequirements(const Trident::MemoryAllocator* memoryAllocator, ref<Trident::Buffer> buffer) {
		const ref<VK::Buffer> _buffer = ref_cast<VK::Buffer>(buffer);

		return *pointer_cast<const MemoryRequirements*>(&_buffer->memoryRequirements);
	}

	MemoryRequirements VK::getImageMemoryRequirements(const Trident::MemoryAllocator* memoryAllocator, ref<Trident::Image> image) {
		const ref<VK::Image> _image = ref_cast<VK::Image>(image);

		return *pointer_cast<const MemoryRequirements*>(&_image->memoryRequirements);
	}

	MemorySuballocation VK::suballocateBufferMemory(Trident::MemoryAllocator* memoryAllocator, uint32 allocationIndex, ref<Trident::Buffer> buffer) {
		const VK::Device* _device = static_cast<const VK::Device*>(memoryAllocator->device);
		const ref<VK::Buffer> _buffer = ref_cast<VK::Buffer>(buffer);
		const MemoryAllocation& memoryAllocation = memoryAllocator->allocations[allocationIndex];

		MemorySuballocation memorySuballocation = memoryAllocator->suballocate(allocationIndex, _buffer->memoryRequirements.size, _buffer->memoryRequirements.alignment);
		memorySuballocation.memoryProperties = buffer->getMemorySuballocation().memoryProperties;

		const VkResult result = _device->vkBindBufferMemory(_device->device, _buffer->buffer, *pointer_cast<const VkDeviceMemory*>(&memoryAllocation.memory), memorySuballocation.offset);

		ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to bind buffer memory ([]).", string_VkResult(result));

		return memorySuballocation;
	}

	MemorySuballocation VK::suballocateImageMemory(Trident::MemoryAllocator* memoryAllocator, uint32 allocationIndex, ref<Trident::Image> image) {
		const VK::Device* _device = static_cast<const VK::Device*>(memoryAllocator->device);
		const ref<VK::Image> _image = ref_cast<VK::Image>(image);
		const MemoryAllocation& memoryAllocation = memoryAllocator->allocations[allocationIndex];

		MemorySuballocation memorySuballocation = memoryAllocator->suballocate(allocationIndex, _image->memoryRequirements.size, _image->memoryRequirements.alignment);
		memorySuballocation.memoryProperties = image->getMemorySuballocation().memoryProperties;

		const VkResult result = _device->vkBindImageMemory(_device->device, _image->image, *pointer_cast<const VkDeviceMemory*>(&memoryAllocation.memory), memorySuballocation.offset);

		ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to bind image memory ([]).", string_VkResult(result));

		return memorySuballocation;
	}
}