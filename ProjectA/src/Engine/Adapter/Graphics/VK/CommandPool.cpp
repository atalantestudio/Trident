// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/VK/CommandPool.hpp"
#include "Engine/Adapter/Graphics/VK/Device.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace ProjectA {
	VK::CommandPool::CommandPool(const VK::Device& device, uint32 queueFamilyIndex) :
		device(device)
	{
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		VkCommandPoolCreateInfo commandPoolCreateInfo{};
		commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		commandPoolCreateInfo.pNext = nullptr;
		commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		commandPoolCreateInfo.queueFamilyIndex = queueFamilyIndex;

		const VkResult result = vkCreateCommandPool(device.device, &commandPoolCreateInfo, nullptr, &commandPool);

		ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to create command pool ([]).", string_VkResult(result));
	}

	VK::CommandPool::~CommandPool() {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		vkDestroyCommandPool(device.device, commandPool, nullptr);
	}
}