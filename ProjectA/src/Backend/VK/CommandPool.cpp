#include "pch.hpp"

#include "Backend/VK/CommandPool.hpp"

namespace ProjectA {
	VulkanCommandPool::VulkanCommandPool(VkDevice device, uint32 queueFamilyIndex) :
		device(device)
	{
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		VkCommandPoolCreateInfo commandPoolCreateInfo{};
		commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		commandPoolCreateInfo.pNext = nullptr;
		commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		commandPoolCreateInfo.queueFamilyIndex = queueFamilyIndex;

		const VkResult result = vkCreateCommandPool(device, &commandPoolCreateInfo, nullptr, &object);

		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to create Vulkan command pool ([]).", string_VkResult(result));
	}

	VulkanCommandPool::~VulkanCommandPool() {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		vkDestroyCommandPool(device, object, nullptr);
	}
}