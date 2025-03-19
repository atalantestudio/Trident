#pragma once

#include "Engine/Renderer/TextureType.hpp"

namespace ProjectA {
	static VkImageType getVkImageType(TextureType textureType) {
		static constexpr VkImageType vkImageTypes[] = {
			VK_IMAGE_TYPE_2D,
		};

		return vkImageTypes[static_cast<uint8>(textureType)];
	}

	static VkImageViewType getVkImageViewType(TextureType textureType) {
		static constexpr VkImageViewType vkImageViewTypes[] = {
			VK_IMAGE_VIEW_TYPE_2D,
		};

		return vkImageViewTypes[static_cast<uint8>(textureType)];
	}
}