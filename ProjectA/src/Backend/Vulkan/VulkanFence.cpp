#include "pch.hpp"

#include "Backend/Vulkan/VulkanFence.hpp"

namespace ProjectA {
	VulkanFence::VulkanFence(VkDevice device) :
		device(device)
	{
		PROFILE_SCOPE(FUNC_SIG);

		VkFenceCreateInfo fenceCreateInfo{};
		fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceCreateInfo.pNext = nullptr;
		fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		const VkResult result = vkCreateFence(device, &fenceCreateInfo, nullptr, &object);

		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to create Vulkan fence ([]).", string_VkResult(result));
	}

	VulkanFence::~VulkanFence() {
		PROFILE_SCOPE(FUNC_SIG);

		vkDestroyFence(device, object, nullptr);
	}

	bool VulkanFence::isSignaled() const {
		const VkResult result = vkGetFenceStatus(device, object);

		DEBUG_ASSERT(result != VK_ERROR_DEVICE_LOST);

		return !static_cast<bool>(result);
	}
}