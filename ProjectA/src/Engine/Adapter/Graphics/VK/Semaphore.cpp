// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/VK/Semaphore.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace ProjectA {
	VK::Semaphore::Semaphore(const VK::Device& device) :
		device(device)
	{
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		VkSemaphoreCreateInfo semaphoreCreateInfo{};
		semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		semaphoreCreateInfo.pNext = nullptr;
		semaphoreCreateInfo.flags = 0;

		const VkResult result = vkCreateSemaphore(device.device, &semaphoreCreateInfo, nullptr, &semaphore);

		ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to create semaphore ([]).", string_VkResult(result));
	}

	VK::Semaphore::~Semaphore() {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		vkDestroySemaphore(device.device, semaphore, nullptr);
	}
}