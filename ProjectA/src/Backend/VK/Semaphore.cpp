#include "pch.hpp"

#include "Backend/VK/Semaphore.hpp"

namespace ProjectA {
	VulkanSemaphore::VulkanSemaphore(VkDevice device) :
		device(device)
	{
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		VkSemaphoreCreateInfo semaphoreCreateInfo{};
		semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		semaphoreCreateInfo.pNext = nullptr;
		semaphoreCreateInfo.flags = 0;

		const VkResult result = vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &object);

		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to create Vulkan semaphore ([]).", string_VkResult(result));
	}

	VulkanSemaphore::~VulkanSemaphore() {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		vkDestroySemaphore(device, object, nullptr);
	}
}