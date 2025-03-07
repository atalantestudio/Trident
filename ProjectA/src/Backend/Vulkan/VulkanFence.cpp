#include "pch.hpp"

#include "Backend/Vulkan/VulkanFence.hpp"

namespace ProjectA {
	VulkanFence::VulkanFence(const VkDevice device) :
		device(device)
	{
		PROFILE_SCOPE(FUNC_SIG);

		ENGINE_TRACE("[VulkanFence]");

		VkFenceCreateInfo fenceCreateInfo{};
		fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceCreateInfo.pNext = nullptr;
		fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		if (vkCreateFence(device, &fenceCreateInfo, nullptr, &object) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create Vulkan fence.");
		}
	}

	VulkanFence::~VulkanFence() {
		PROFILE_SCOPE(FUNC_SIG);

		ENGINE_TRACE("[~VulkanFence]");

		vkDestroyFence(device, object, nullptr);
	}
}