// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#if defined(DEBUG_OBJECT_NAME)
	#include "Engine/Adapter/Graphics/VK/Debug/ObjectName.hpp"
#endif

#include "Engine/Adapter/Graphics/VK/Fence.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace Trident {
	VK::Fence::Fence(const Trident::Device* device) :
		Trident::Fence(device)
	{}

	ref<Trident::Fence> VK::createFence(const Trident::Device* device, const FenceDescriptor& descriptor) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const VK::Device* _device = static_cast<const VK::Device*>(device);
		VkResult result;

		ref<VK::Fence> fence = allocateRef<VK::Fence>(device);

		VkFenceCreateInfo fenceCreateInfo{};
		fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceCreateInfo.pNext = nullptr;
		fenceCreateInfo.flags = 0;

		if (descriptor.signaled) {
			fenceCreateInfo.flags |= VK_FENCE_CREATE_SIGNALED_BIT;
		}

		result = _device->vkCreateFence(_device->device, &fenceCreateInfo, nullptr, &fence->fence);

		ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to create fence ([]).", static_cast<sint32>(result));

		#if defined(DEBUG_OBJECT_NAME)
			setObjectName(_device, (uint64) fence->fence, VK_OBJECT_TYPE_FENCE, descriptor.name);
		#endif

		return fence;
	}

	void VK::destroyFence(const Trident::Device* device, const Trident::Fence* fence) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const VK::Device* _device = static_cast<const VK::Device*>(device);
		const VK::Fence* _fence = static_cast<const VK::Fence*>(fence);

		_device->vkDestroyFence(_device->device, _fence->fence, nullptr);
	}
}