#include "pch.hpp"

#include "Platform/Vulkan/VulkanSemaphore.hpp"

namespace ProjectA {
	VulkanSemaphore::VulkanSemaphore(const VkDevice device) :
		device(device)
	{
		PROFILE_SCOPE(FUNC_SIG);

		ENGINE_TRACE("[VulkanSemaphore]");

		VkSemaphoreCreateInfo semaphoreCreateInfo{};
		semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		semaphoreCreateInfo.pNext = nullptr;
		semaphoreCreateInfo.flags = 0;

		if (vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &object) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create Vulkan semaphore.");
		}
	}

	VulkanSemaphore::~VulkanSemaphore() {
		PROFILE_SCOPE(FUNC_SIG);

		ENGINE_TRACE("[~VulkanSemaphore]");

		vkDestroySemaphore(device, object, nullptr);
	}
}