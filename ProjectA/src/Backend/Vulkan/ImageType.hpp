#pragma once

#include "Engine/Renderer/ImageType.hpp"

namespace ProjectA {
	static VkImageType getVkImageType(ImageType imageType) {
		static constexpr VkImageType vkImageTypes[] = {
			VK_IMAGE_TYPE_2D,
		};

		return vkImageTypes[static_cast<uint8>(imageType)];
	}

	static VkImageViewType getVkImageViewType(ImageType imageType) {
		static constexpr VkImageViewType vkImageViewTypes[] = {
			VK_IMAGE_VIEW_TYPE_2D,
		};

		return vkImageViewTypes[static_cast<uint8>(imageType)];
	}
}