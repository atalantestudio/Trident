// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/VK/Buffer.hpp"
#include "Engine/Adapter/Graphics/VK/BufferUsage.hpp"
#include "Engine/Adapter/Graphics/VK/Device.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace Trident {
	VK::Buffer::Buffer(const BufferDescriptor& descriptor, const VK::Device& device) :
		Trident::Buffer(descriptor),
		device(device)
	{
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		VkBufferCreateInfo bufferCreateInfo{};
		bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferCreateInfo.pNext = nullptr;
		bufferCreateInfo.flags = 0;
		bufferCreateInfo.size = size;
		bufferCreateInfo.usage = getBufferUsageFlagBits(descriptor.usage);
		bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		bufferCreateInfo.queueFamilyIndexCount = 0;
		bufferCreateInfo.pQueueFamilyIndices = nullptr;

		const VkResult result = vkCreateBuffer(device.device, &bufferCreateInfo, nullptr, &buffer);

		ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to create buffer ([]).", string_VkResult(result));

		VkMemoryRequirements memoryRequirements;

		vkGetBufferMemoryRequirements(device.device, buffer, &memoryRequirements);

		VkMemoryPropertyFlags memoryPropertyFlags = static_cast<VkMemoryPropertyFlags>(descriptor.memoryProperties);
		uint32 memoryTypeIndex = MemoryAllocator::findSuitableMemoryType(memoryRequirements, memoryPropertyFlags);

		if ((memoryPropertyFlags == (VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)) && memoryTypeIndex == UNDEFINED_MEMORY_TYPE_INDEX) {
			ADAPTER_WARNING("Failed to find DEVICE_LOCAL|HOST_VISIBLE memory for image. Falling back to HOST_VISIBLE|HOST_COHERENT memory.");

			memoryPropertyFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
			memoryTypeIndex = MemoryAllocator::findSuitableMemoryType(memoryRequirements, memoryPropertyFlags);
		}

		if ((memoryPropertyFlags == (VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)) && memoryTypeIndex == UNDEFINED_MEMORY_TYPE_INDEX) {
			ADAPTER_WARNING("Failed to find HOST_VISIBLE|HOST_COHERENT memory for image. Falling back to DEVICE_LOCAL memory.");

			memoryPropertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
			memoryTypeIndex = MemoryAllocator::findSuitableMemoryType(memoryRequirements, memoryPropertyFlags);
		}

		ADAPTER_ASSERT(memoryTypeIndex != UNDEFINED_MEMORY_TYPE_INDEX, "Undefined memory type index.");

		memorySuballocation = MemoryAllocator::suballocateBuffer(device.device, buffer, memoryTypeIndex, memoryRequirements);
	}

	VK::Buffer::~Buffer() {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		vkDestroyBuffer(device.device, buffer, nullptr);

		MemoryAllocator::freeSuballocation(device.device, memorySuballocation.allocationIndex);
	}

	void VK::Buffer::unmap() const {
		MemoryAllocator::unmapSuballocation(device.device, memorySuballocation.allocationIndex);
	}

	void* VK::Buffer::getMappedRange(uint64 offset, uint64 size) const {
		return MemoryAllocator::mapSuballocation(device.device, memorySuballocation, offset, size);
	}
}