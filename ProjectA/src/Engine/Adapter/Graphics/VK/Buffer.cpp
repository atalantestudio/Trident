#include "pch.hpp"

#include "Engine/Adapter/Graphics/VK/Buffer.hpp"
#include "Engine/Adapter/Graphics/VK/BufferUsage.hpp"

namespace ProjectA {
	VulkanBuffer::VulkanBuffer(VkDevice device, const BufferDescriptor& descriptor) :
		Buffer(descriptor),
		device(device)
	{
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		VkBufferCreateInfo bufferCreateInfo{};
		bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferCreateInfo.pNext = nullptr;
		bufferCreateInfo.flags = 0;
		bufferCreateInfo.size = size;
		bufferCreateInfo.usage = getVkBufferUsageFlagBits(descriptor.usage);
		bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		bufferCreateInfo.queueFamilyIndexCount = 0;
		bufferCreateInfo.pQueueFamilyIndices = nullptr;

		const VkResult result = vkCreateBuffer(device, &bufferCreateInfo, nullptr, &object);

		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to create Vulkan buffer ([]).", string_VkResult(result));

		VkMemoryRequirements memoryRequirements;

		vkGetBufferMemoryRequirements(device, object, &memoryRequirements);

		VkMemoryPropertyFlags memoryPropertyFlags = static_cast<VkMemoryPropertyFlags>(descriptor.memoryProperties);
		uint32 memoryTypeIndex = MemoryAllocator::findSuitableMemoryType(memoryRequirements, memoryPropertyFlags);

		if ((memoryPropertyFlags == (VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)) && memoryTypeIndex == UNDEFINED_MEMORY_TYPE_INDEX) {
			ENGINE_WARNING("Failed to find DEVICE_LOCAL | HOST_VISIBLE memory for Vulkan image. Falling back to HOST_VISIBLE | HOST_COHERENT memory.");

			memoryPropertyFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
			memoryTypeIndex = MemoryAllocator::findSuitableMemoryType(memoryRequirements, memoryPropertyFlags);
		}

		if ((memoryPropertyFlags == (VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)) && memoryTypeIndex == UNDEFINED_MEMORY_TYPE_INDEX) {
			ENGINE_WARNING("Failed to find HOST_VISIBLE | HOST_COHERENT memory for Vulkan image. Falling back to DEVICE_LOCAL memory.");

			memoryPropertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
			memoryTypeIndex = MemoryAllocator::findSuitableMemoryType(memoryRequirements, memoryPropertyFlags);
		}

		DEBUG_ASSERT(memoryTypeIndex != UNDEFINED_MEMORY_TYPE_INDEX);

		memorySuballocation = MemoryAllocator::suballocateBuffer(device, object, memoryTypeIndex, memoryRequirements);
	}

	VulkanBuffer::~VulkanBuffer() {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		vkDestroyBuffer(device, object, nullptr);

		MemoryAllocator::freeSuballocation(device, memorySuballocation.allocationIndex);
	}

	void VulkanBuffer::unmap() const {
		MemoryAllocator::unmapSuballocation(device, memorySuballocation.allocationIndex);
	}

	void* VulkanBuffer::getMappedRange(uint64 offset, uint64 size) const {
		return MemoryAllocator::mapSuballocation(device, memorySuballocation, offset, size);
	}
}