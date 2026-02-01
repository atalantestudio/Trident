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

		VkResult result;

		VkFenceCreateInfo fenceCreateInfo{};
		fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceCreateInfo.pNext = nullptr;
		fenceCreateInfo.flags = 0;

		if (descriptor.signaled) {
			fenceCreateInfo.flags |= VK_FENCE_CREATE_SIGNALED_BIT;
		}

		result = vkCreateFence(device.device, &fenceCreateInfo, nullptr, &fence);

		ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to create fence ([]).", string_VkResult(result));

		#if defined(BUILD_MODE_DEBUG)
			if (descriptor.name.count() > 0) {
				VkDebugUtilsObjectNameInfoEXT debugUtilsObjectNameInfo{};
				debugUtilsObjectNameInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
				debugUtilsObjectNameInfo.pNext = nullptr;
				debugUtilsObjectNameInfo.objectType = VK_OBJECT_TYPE_FENCE;
				debugUtilsObjectNameInfo.objectHandle = (uint64) fence;
				debugUtilsObjectNameInfo.pObjectName = &descriptor.name[0];

				result = vkSetDebugUtilsObjectNameEXT(device.device, &debugUtilsObjectNameInfo);

				ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to set object name ([]).", string_VkResult(result));
			}
		#endif
	}

	VK::Fence::~Fence() {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		vkDestroyFence(device.device, fence, nullptr);
	}
}