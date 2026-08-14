// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/VK/CommandPool.hpp"
#include "Engine/Adapter/Graphics/VK/Device.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace Trident {
	VK::CommandPool::CommandPool(const Trident::Device* device) :
		Trident::CommandPool(device)
	{}

	ref<Trident::CommandPool> VK::createCommandPool(const Trident::Device* device, uint32 queueFamilyIndex, QueueFamilyCapability queueFamilyCapability) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const VK::Device* _device = static_cast<const VK::Device*>(device);
		VkResult result;

		ref<VK::CommandPool> commandPool = allocateRef<VK::CommandPool>(device);
		commandPool->pipelineStageFlags = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;

		if (queueFamilyCapability == QueueFamilyCapability::TRANSFER) {
			commandPool->pipelineStageFlags = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}

		VkCommandPoolCreateInfo commandPoolCreateInfo{};
		commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		commandPoolCreateInfo.pNext = nullptr;
		commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		commandPoolCreateInfo.queueFamilyIndex = queueFamilyIndex;

		result = _device->vkCreateCommandPool(_device->device, &commandPoolCreateInfo, nullptr, &commandPool->commandPool);

		ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to create command pool ([]).", string_VkResult(result));

		return commandPool;
	}

	void VK::destroyCommandPool(const Trident::Device* device, const Trident::CommandPool* commandPool) {
		// The command pool is destroyed by the device.
	}
}