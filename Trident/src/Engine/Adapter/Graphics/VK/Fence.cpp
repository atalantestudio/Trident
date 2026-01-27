// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/VK/Fence.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace Trident {
	VK::Fence::Fence(const FenceDescriptor& descriptor, const VK::Device& device) :
		Trident::Fence(descriptor),
		device(device)
	{
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		VkFenceCreateInfo fenceCreateInfo{};
		fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceCreateInfo.pNext = nullptr;
		fenceCreateInfo.flags = 0;

		if (descriptor.signaled) {
			fenceCreateInfo.flags |= VK_FENCE_CREATE_SIGNALED_BIT;
		}

		const VkResult result = vkCreateFence(device.device, &fenceCreateInfo, nullptr, &fence);

		ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to create fence ([]).", string_VkResult(result));
	}

	VK::Fence::~Fence() {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		vkDestroyFence(device.device, fence, nullptr);
	}
}