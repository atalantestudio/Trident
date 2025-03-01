#include "pch.hpp"

#include "Platform/Vulkan/VulkanCommandPool.hpp"

namespace ProjectA {
	VulkanCommandPool::VulkanCommandPool(const VulkanPhysicalDevice& physicalDevice, const VkDevice device) :
		device(device)
	{
		VkCommandPoolCreateInfo commandPoolCreateInfo{};
		commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		commandPoolCreateInfo.pNext = nullptr;
		commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		commandPoolCreateInfo.queueFamilyIndex = physicalDevice.graphicsQueueFamilyIndex;

		if (vkCreateCommandPool(device, &commandPoolCreateInfo, nullptr, &object) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create Vulkan command pool.");
		}
	}

	VulkanCommandPool::~VulkanCommandPool() {
		vkDestroyCommandPool(device, object, nullptr);
	}
}