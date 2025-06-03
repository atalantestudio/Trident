#include "pch.hpp"

#include "Backend/Vulkan/VulkanBuffer.hpp"

#include "Backend/Vulkan/BufferUsage.hpp"

namespace ProjectA {
	VulkanBuffer::VulkanBuffer(VkDevice device, const BufferDescriptor& descriptor) :
		Buffer(descriptor.size),
		device(device)
	{
		PROFILE_SCOPE(FUNC_SIG);

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
		uint32 memoryTypeIndex = MemoryAllocator::findMemoryType(memoryRequirements, memoryPropertyFlags);

		if ((memoryPropertyFlags == (VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)) && memoryTypeIndex == UNDEFINED_MEMORY_TYPE_INDEX) {
			ENGINE_WARN("Failed to find DEVICE_LOCAL | HOST_VISIBLE memory for Vulkan image. Falling back to HOST_VISIBLE | HOST_COHERENT memory.");

			memoryPropertyFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
			memoryTypeIndex = MemoryAllocator::findMemoryType(memoryRequirements, memoryPropertyFlags);
		}

		if ((memoryPropertyFlags == (VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)) && memoryTypeIndex == UNDEFINED_MEMORY_TYPE_INDEX) {
			ENGINE_WARN("Failed to find HOST_VISIBLE | HOST_COHERENT memory for Vulkan image. Falling back to DEVICE_LOCAL memory.");

			memoryPropertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
			memoryTypeIndex = MemoryAllocator::findMemoryType(memoryRequirements, memoryPropertyFlags);
		}

		DEBUG_ASSERT(memoryTypeIndex != UNDEFINED_MEMORY_TYPE_INDEX);

		memorySuballocation = MemoryAllocator::suballocateBuffer(device, object, memoryTypeIndex, memoryRequirements.size);
	}

	VulkanBuffer::~VulkanBuffer() {
		PROFILE_SCOPE(FUNC_SIG);

		vkDestroyBuffer(device, object, nullptr);

		MemoryAllocator::freeSuballocation(device, memorySuballocation.blockIndex);
	}

	void VulkanBuffer::unmap() const {
		MemoryAllocator::unmapSuballocation(device, memorySuballocation.blockIndex);
	}

	void* VulkanBuffer::getMappedRange(uint64 offset, uint64 size) const {
		return MemoryAllocator::mapSuballocation(device, memorySuballocation, size, offset);
	}
}