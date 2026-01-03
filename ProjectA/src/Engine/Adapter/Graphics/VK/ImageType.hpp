#pragma once

#include "Engine/API/Rendering/ImageType.hpp"

namespace ProjectA {
	static VkImageType getVkImageType(ImageType imageType) {
		static constexpr VkImageType vkImageTypes[] = {
			VK_IMAGE_TYPE_2D,
		};

		return vkImageTypes[static_cast<uint8>(imageType)];
	}

	static VkImageViewType getVkImageViewType(VkImageType vkImageType, uint32 layerCount) {
		if (vkImageType == VK_IMAGE_TYPE_2D) {
			if (layerCount > 1) {
				return VK_IMAGE_VIEW_TYPE_2D_ARRAY;
			}

			return VK_IMAGE_VIEW_TYPE_2D;
		}

		DEBUG_ASSERT(false);

		return VK_IMAGE_VIEW_TYPE_MAX_ENUM;
	}
}