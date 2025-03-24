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

		ENGINE_TRACE("[VulkanBuffer]");

		VkBufferCreateInfo bufferCreateInfo{};
		bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferCreateInfo.pNext = nullptr;
		bufferCreateInfo.flags = 0;
		bufferCreateInfo.size = size;
		bufferCreateInfo.usage = getVkBufferUsageFlagBits(descriptor.usage);
		bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		bufferCreateInfo.queueFamilyIndexCount = 0;
		bufferCreateInfo.pQueueFamilyIndices = nullptr;

		if (vkCreateBuffer(device.object, &bufferCreateInfo, nullptr, &object) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create buffer.");
		}

		VkMemoryRequirements memoryRequirements;

		vkGetBufferMemoryRequirements(device.object, object, &memoryRequirements);

		// HOST_COHERENT avoid caching.
		const uint32 memoryTypeIndex = device.findMemoryType(memoryRequirements, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		VkMemoryAllocateInfo memoryAllocateInfo{};
		memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		memoryAllocateInfo.pNext = nullptr;
		memoryAllocateInfo.allocationSize = memoryRequirements.size;
		memoryAllocateInfo.memoryTypeIndex = memoryTypeIndex;

		if (vkAllocateMemory(device.object, &memoryAllocateInfo, nullptr, &memory) != VK_SUCCESS) {
			throw std::runtime_error("Failed to allocate memory.");
		}

		vkBindBufferMemory(device.object, object, memory, 0);
	}

	VulkanBuffer::~VulkanBuffer() {
		PROFILE_SCOPE(FUNC_SIG);

		ENGINE_TRACE("[~VulkanBuffer]");

		vkDestroyBuffer(device.object, object, nullptr);
		vkFreeMemory(device.object, memory, nullptr);
	}

	BufferMappedRange VulkanBuffer::map(uint64 offset, uint64 size) const {
		BufferMappedRange mappedRange;

		if (vkMapMemory(device.object, memory, offset, size, 0, &mappedRange) != VK_SUCCESS) {
			throw std::runtime_error("Failed to map buffer memory.");
		}

		return mappedRange;
	}

	void VulkanBuffer::unmap() const {
		vkUnmapMemory(device.object, memory);
	}
}