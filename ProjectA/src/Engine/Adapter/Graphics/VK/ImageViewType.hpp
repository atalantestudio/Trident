// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace ProjectA::VK {
	inline VkImageViewType getImageViewType(VkImageType vkImageType, uint32 layerCount) {
		if (vkImageType == VK_IMAGE_TYPE_2D) {
			if (layerCount > 1) {
				return VK_IMAGE_VIEW_TYPE_2D_ARRAY;
			}

			return VK_IMAGE_VIEW_TYPE_2D;
		}

		ADAPTER_ASSERT(false, "Unsupported image type.");

		return VK_IMAGE_VIEW_TYPE_MAX_ENUM;
	}
}