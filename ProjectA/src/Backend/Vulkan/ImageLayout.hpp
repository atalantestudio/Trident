#pragma once

namespace ProjectA {
	static VkImageLayout getVkImageLayout(ImageLayout imageLayout) {
		static constexpr VkImageLayout vkImageLayouts[] = {
			VK_IMAGE_LAYOUT_UNDEFINED,
			VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
			VK_IMAGE_LAYOUT_GENERAL,
		};

		return vkImageLayouts[static_cast<uint8>(imageLayout)];
	}

	static VkAccessFlags getVkAccessFlags(ImageLayout imageLayout) {
		static constexpr VkAccessFlags vkAccessFlags[] = {
			VK_ACCESS_NONE,
			VK_ACCESS_TRANSFER_READ_BIT,
			VK_ACCESS_TRANSFER_WRITE_BIT,
			VK_ACCESS_SHADER_READ_BIT,
			0,
		};

		return vkAccessFlags[static_cast<uint8>(imageLayout)];
	}
}