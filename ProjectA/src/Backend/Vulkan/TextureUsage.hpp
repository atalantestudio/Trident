#pragma once

#include "Engine/Renderer/TextureUsage.hpp"

namespace ProjectA {
	static VkImageUsageFlagBits getVkImageUsageFlagBits(TextureUsage textureUsage) {
		static constexpr VkImageUsageFlagBits vkImageUsageFlagBits[] = {
			VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
			VK_IMAGE_USAGE_TRANSFER_DST_BIT,
			VK_IMAGE_USAGE_SAMPLED_BIT,
			VK_IMAGE_USAGE_STORAGE_BIT,
			VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
			VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
		};

		return vkImageUsageFlagBits[static_cast<sint32>(textureUsage)];
	}
}