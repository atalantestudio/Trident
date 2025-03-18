#pragma once

#include "Engine/Renderer/TextureType.hpp"

namespace ProjectA {
	static VkImageType getVkImageType(TextureType textureType) {
		static constexpr VkImageType vkImageTypes[] = {
			VK_IMAGE_TYPE_2D,
		};

		return vkImageTypes[static_cast<sint32>(textureType)];
	}
}