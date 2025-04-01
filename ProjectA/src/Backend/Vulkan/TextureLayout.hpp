#pragma once

#include <vulkan/vulkan.h>

#include "Engine/Renderer/TextureLayout.hpp"

namespace ProjectA {
	static VkImageLayout getVkImageLayout(TextureLayout textureLayout) {
		static constexpr VkImageLayout vkImageLayouts[] = {
			VK_IMAGE_LAYOUT_UNDEFINED,
			VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
			VK_IMAGE_LAYOUT_GENERAL,
		};

		return vkImageLayouts[static_cast<uint8>(textureLayout)];
	}
}