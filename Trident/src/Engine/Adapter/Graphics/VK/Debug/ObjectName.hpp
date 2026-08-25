// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/VK/Device.hpp"

namespace Trident::VK {
	inline void setObjectName(const VK::Device* device, uint64 handle, VkObjectType type, view<char8> name) {
		VkDebugUtilsObjectNameInfoEXT debugUtilsObjectNameInfo{};
		debugUtilsObjectNameInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
		debugUtilsObjectNameInfo.pNext = nullptr;
		debugUtilsObjectNameInfo.objectType = type;
		debugUtilsObjectNameInfo.objectHandle = handle;
		debugUtilsObjectNameInfo.pObjectName = &name[0];

		const VkResult result = device->vkSetDebugUtilsObjectNameEXT(device->device, &debugUtilsObjectNameInfo);

		ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to set object name ([]).", string_VkResult(result));
	}
}