#include "pch.hpp"

#include "Backend/Vulkan/VulkanCommandPool.hpp"

namespace ProjectA {
	VulkanCommandPool::VulkanCommandPool(const VulkanPhysicalDevice& physicalDevice, VkDevice device) :
		device(device)
	{
		PROFILE_SCOPE(FUNC_SIG);

		VkCommandPoolCreateInfo commandPoolCreateInfo{};
		commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		commandPoolCreateInfo.pNext = nullptr;
		commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		commandPoolCreateInfo.queueFamilyIndex = physicalDevice.graphicsQueueFamilyIndex;

		const VkResult result = vkCreateCommandPool(device, &commandPoolCreateInfo, nullptr, &object);

		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to create Vulkan command pool (code: []).", result);
	}

	VulkanCommandPool::~VulkanCommandPool() {
		PROFILE_SCOPE(FUNC_SIG);

		vkDestroyCommandPool(device, object, nullptr);
	}
}