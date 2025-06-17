#pragma once

namespace ProjectA {
	static VkFormat getVkFormat(ImageFormat imageFormat) {
		static constexpr VkFormat vkFormats[] = {
			VK_FORMAT_R8_UNORM,
			VK_FORMAT_R8G8B8_UNORM,
			VK_FORMAT_B8G8R8A8_UNORM,
			VK_FORMAT_R8G8B8A8_SRGB,
			VK_FORMAT_R8G8B8A8_UNORM,
			VK_FORMAT_D32_SFLOAT,
			VK_FORMAT_D32_SFLOAT_S8_UINT,
			VK_FORMAT_D24_UNORM_S8_UINT,
		};

		return vkFormats[static_cast<uint8>(imageFormat)];
	}

	static bool isDepthFormat(VkFormat format) {
		return (
			format == VK_FORMAT_D32_SFLOAT ||
			format == VK_FORMAT_D32_SFLOAT_S8_UINT ||
			format == VK_FORMAT_D24_UNORM_S8_UINT
		);
	}

	static bool isStencilFormat(VkFormat format) {
		return (
			format == VK_FORMAT_S8_UINT ||
			format == VK_FORMAT_D16_UNORM_S8_UINT ||
			format == VK_FORMAT_D24_UNORM_S8_UINT ||
			format == VK_FORMAT_D32_SFLOAT_S8_UINT
		);
	}
}