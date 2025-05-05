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

		// TODO: If fences are signaled by default, this behavior should be extended to the OpenGL backend.
		fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		const VkResult result = vkCreateFence(device, &fenceCreateInfo, nullptr, &object);

		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to create Vulkan fence (code: []).", result);
	}

	VulkanFence::~VulkanFence() {
		PROFILE_SCOPE(FUNC_SIG);

		vkDestroyFence(device, object, nullptr);
	}
}