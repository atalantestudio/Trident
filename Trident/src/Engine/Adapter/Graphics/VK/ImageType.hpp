// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/ImageType.hpp"

namespace Trident::VK {
	inline VkImageType getImageType(ImageType imageType) {
		static constexpr VkImageType IMAGE_TYPES[] = {
			VK_IMAGE_TYPE_2D,
		};

		ADAPTER_ASSERT(imageType < ImageType::count, "Invalid image type.");

		return IMAGE_TYPES[static_cast<uint32>(imageType)];
	}
}