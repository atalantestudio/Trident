#pragma once

#include "Engine/Renderer/ImageUsage.hpp"

namespace ProjectA {
	static VkImageUsageFlagBits getVkImageUsageFlagBits(ImageUsage imageUsage) {
		VkImageUsageFlags vkImageUsageFlags = 0;

		if (imageUsage & ImageUsage::TRANSFER_SRC) {
			vkImageUsageFlags |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
		}

		if (imageUsage & ImageUsage::TRANSFER_DST) {
			vkImageUsageFlags |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		}

		if (imageUsage & ImageUsage::SAMPLED) {
			vkImageUsageFlags |= VK_IMAGE_USAGE_SAMPLED_BIT;
		}

		if (imageUsage & ImageUsage::STORAGE) {
			vkImageUsageFlags |= VK_IMAGE_USAGE_STORAGE_BIT;
		}

		if (imageUsage & ImageUsage::COLOR_ATTACHMENT) {
			vkImageUsageFlags |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		}

		if (imageUsage & ImageUsage::DEPTH_STENCIL_ATTACHMENT) {
			vkImageUsageFlags |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
		}

		return static_cast<VkImageUsageFlagBits>(vkImageUsageFlags);
	}
}