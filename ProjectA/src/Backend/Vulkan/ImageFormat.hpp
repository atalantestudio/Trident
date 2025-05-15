#pragma once

namespace ProjectA {
	static VkFormat getVkFormat(ImageFormat imageFormat) {
		static constexpr VkFormat vkFormats[] = {
			VK_FORMAT_R8_UNORM,
			VK_FORMAT_R8G8B8A8_SRGB,
			VK_FORMAT_R8G8B8A8_UNORM,
		};

		return vkFormats[static_cast<uint8>(imageFormat)];
	}
}