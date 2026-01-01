#include "pch.hpp"

#include "Backend/VK/Fence.hpp"

namespace ProjectA {
	VulkanFence::VulkanFence(VkDevice device) :
		device(device)
	{
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		VkFenceCreateInfo fenceCreateInfo{};
		fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceCreateInfo.pNext = nullptr;
		fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		const VkResult result = vkCreateFence(device, &fenceCreateInfo, nullptr, &object);

		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to create Vulkan fence ([]).", string_VkResult(result));
	}

	VulkanFence::~VulkanFence() {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		vkDestroyFence(device, object, nullptr);
	}
}