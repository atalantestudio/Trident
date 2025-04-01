#pragma once

#include "Engine/Renderer/TextureUsage.hpp"

namespace ProjectA {
	static VkImageUsageFlagBits getVkImageUsageFlagBits(TextureUsage textureUsage) {
		VkImageUsageFlags vkImageUsageFlags = 0;

		if (textureUsage & TextureUsage::TRANSFER_SRC) {
			vkImageUsageFlags |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
		}

		if (textureUsage & TextureUsage::TRANSFER_DST) {
			vkImageUsageFlags |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		}

		if (textureUsage & TextureUsage::SAMPLED) {
			vkImageUsageFlags |= VK_IMAGE_USAGE_SAMPLED_BIT;
		}

		if (textureUsage & TextureUsage::STORAGE) {
			vkImageUsageFlags |= VK_IMAGE_USAGE_STORAGE_BIT;
		}

		if (textureUsage & TextureUsage::COLOR_ATTACHMENT) {
			vkImageUsageFlags |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		}

		if (textureUsage & TextureUsage::DEPTH_STENCIL_ATTACHMENT) {
			vkImageUsageFlags |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
		}

		return static_cast<VkImageUsageFlagBits>(vkImageUsageFlags);
	}
}