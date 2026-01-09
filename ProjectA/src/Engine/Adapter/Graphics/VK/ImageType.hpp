// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/ImageType.hpp"

namespace ProjectA::VK {
	inline VkImageType getImageType(ImageType imageType) {
		static constexpr VkImageType vkImageTypes[] = {
			VK_IMAGE_TYPE_2D,
		};

		return vkImageTypes[static_cast<uint8>(imageType)];
	}
}