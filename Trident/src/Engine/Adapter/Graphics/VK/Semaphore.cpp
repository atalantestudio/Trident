// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/VK/Device.hpp"
#include "Engine/Adapter/Graphics/VK/Semaphore.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace Trident {
	VK::Semaphore::Semaphore(const Trident::Device* device) :
		Trident::Semaphore(device)
	{}

	ref<Trident::Semaphore> VK::createSemaphore(const Trident::Device* device) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const VK::Device* _device = static_cast<const VK::Device*>(device);

		ref<VK::Semaphore> semaphore = allocateRef<VK::Semaphore>(device);
		VkResult result;

		VkSemaphoreCreateInfo semaphoreCreateInfo{};
		semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		semaphoreCreateInfo.pNext = nullptr;
		semaphoreCreateInfo.flags = 0;

		result = _device->vkCreateSemaphore(_device->device, &semaphoreCreateInfo, nullptr, &semaphore->semaphore);

		ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to create semaphore ([]).", static_cast<sint32>(result));

		return semaphore;
	}

	void VK::destroySemaphore(const Trident::Device* device, const Trident::Semaphore* semaphore) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const VK::Device* _device = static_cast<const VK::Device*>(device);
		const VK::Semaphore* _semaphore = static_cast<const VK::Semaphore*>(semaphore);

		_device->vkDestroySemaphore(_device->device, _semaphore->semaphore, nullptr);
	}
}