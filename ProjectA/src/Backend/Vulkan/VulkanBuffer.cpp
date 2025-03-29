#include "pch.hpp"

#include "Backend/Vulkan/BufferUsage.hpp"
#include "Backend/Vulkan/VulkanBuffer.hpp"

namespace ProjectA {
	VulkanBuffer::VulkanBuffer(VkPhysicalDevice physicalDevice, VkCommandPool commandPool, const VulkanDevice& device, const BufferDescriptor& descriptor) :
		device(device),
		size(descriptor.size),
		commandPool(commandPool)
	{
		PROFILE_SCOPE(FUNC_SIG);

		VkResult result;

		VkBufferCreateInfo bufferCreateInfo{};
		bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferCreateInfo.pNext = nullptr;
		bufferCreateInfo.flags = 0;
		bufferCreateInfo.size = size;
		bufferCreateInfo.usage = getVkBufferUsageFlagBits(descriptor.usage);
		bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		bufferCreateInfo.queueFamilyIndexCount = 0;
		bufferCreateInfo.pQueueFamilyIndices = nullptr;

		result = vkCreateBuffer(device.object, &bufferCreateInfo, nullptr, &object);

		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to create Vulkan buffer (code: []).", result);

		VkMemoryRequirements memoryRequirements;

		vkGetBufferMemoryRequirements(device.object, object, &memoryRequirements);

		// HOST_COHERENT avoid caching.
		const uint32 memoryTypeIndex = device.findMemoryType(memoryRequirements, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		VkMemoryAllocateInfo memoryAllocateInfo{};
		memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		memoryAllocateInfo.pNext = nullptr;
		memoryAllocateInfo.allocationSize = memoryRequirements.size;
		memoryAllocateInfo.memoryTypeIndex = memoryTypeIndex;

		result = vkAllocateMemory(device.object, &memoryAllocateInfo, nullptr, &memory);

		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to allocate Vulkan memory (code: []).", result);

		result = vkBindBufferMemory(device.object, object, memory, 0);

		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to bind Vulkan buffer memory (code: []).", result);
	}

	VulkanBuffer::~VulkanBuffer() {
		PROFILE_SCOPE(FUNC_SIG);

		vkDestroyBuffer(device.object, object, nullptr);
		vkFreeMemory(device.object, memory, nullptr);
	}

	BufferMappedRange VulkanBuffer::map(uint64 offset, uint64 size) const {
		BufferMappedRange mappedRange;

		if (size == 0) {
			DEBUGBREAK();
		}

		const VkResult result = vkMapMemory(device.object, memory, offset, size, 0, &mappedRange);

		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to map Vulkan buffer memory (code: []).", result);

		return mappedRange;
	}

	void VulkanBuffer::unmap() const {
		vkUnmapMemory(device.object, memory);
	}
}