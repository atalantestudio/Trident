#include "pch.hpp"

#include "Platform/Vulkan/BufferUsage.hpp"
#include "Platform/Vulkan/VulkanBuffer.hpp"

namespace ProjectA {
	VulkanBuffer::VulkanBuffer(const VkPhysicalDevice physicalDevice, VulkanDevice& device, const BufferDescriptor& descriptor) :
		device(device),
		size(descriptor.size)
	{
		PROFILE_SCOPE(FUNC_SIG);

		VkBufferCreateInfo bufferCreateInfo{};
		bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferCreateInfo.pNext = nullptr;
		bufferCreateInfo.flags = 0;
		bufferCreateInfo.size = size;
		bufferCreateInfo.usage = getVkBufferUsageFlagBits(descriptor.usage);

		// TODO: Automatically determine sharing mode.
		bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		bufferCreateInfo.queueFamilyIndexCount = 0;
		bufferCreateInfo.pQueueFamilyIndices = nullptr;

		if (vkCreateBuffer(device.object, &bufferCreateInfo, nullptr, &object) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create Vulkan buffer.");
		}

		VkMemoryRequirements memoryRequirements;

		vkGetBufferMemoryRequirements(device.object, object, &memoryRequirements);

		device.allocateMemory(memoryRequirements);
	}

	VulkanBuffer::~VulkanBuffer() {
		PROFILE_SCOPE(FUNC_SIG);

		vkDestroyBuffer(device.object, object, nullptr);
	}

	void VulkanBuffer::write(const void* data) const {
		device.writeBuffer(size, data);
	}
}