// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/GraphicsAdapter.hpp"
#include "Engine/API/Graphics/ImageViewType.hpp"

namespace Trident::VK {
	inline VkImageViewType getImageViewType(ImageViewType imageViewType, bool array) {
		static constexpr VkImageViewType IMAGE_VIEW_TYPES[] = {
			VK_IMAGE_VIEW_TYPE_2D,
			VK_IMAGE_VIEW_TYPE_CUBE,
			VK_IMAGE_VIEW_TYPE_2D_ARRAY,
			VK_IMAGE_VIEW_TYPE_CUBE_ARRAY,
		};

		ADAPTER_ASSERT(imageViewType < ImageViewType::count, "Invalid image view type.");

		const uint32 index = static_cast<uint32>(imageViewType) + array * static_cast<uint32>(ImageViewType::count);

		return IMAGE_VIEW_TYPES[index];
	}
}