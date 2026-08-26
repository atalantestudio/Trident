// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/GraphicsAdapter.hpp"
#include "Engine/API/Graphics/ImageViewType.hpp"

namespace Trident::VK {
	inline VkImageViewType getImageViewType(ImageViewType imageViewType, uint32 layerCount) {
		if (imageViewType == ImageViewType::TYPE_CUBE) {
			if (layerCount > 6) {
				return VK_IMAGE_VIEW_TYPE_CUBE_ARRAY;
			}

			return VK_IMAGE_VIEW_TYPE_CUBE;
		}

		if (layerCount > 1) {
			return VK_IMAGE_VIEW_TYPE_2D_ARRAY;
		}

		return VK_IMAGE_VIEW_TYPE_2D;
	}
}