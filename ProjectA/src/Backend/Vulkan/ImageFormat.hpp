#pragma once

namespace ProjectA {
	static VkFormat getVkFormat(ImageFormat imageFormat) {
		static constexpr VkFormat vkFormats[] = {
			VK_FORMAT_R8_UNORM,
			VK_FORMAT_B8G8R8A8_UNORM,
			VK_FORMAT_R8G8B8A8_SRGB,
			VK_FORMAT_R8G8B8A8_UNORM,
			VK_FORMAT_D32_SFLOAT,
			VK_FORMAT_D32_SFLOAT_S8_UINT,
			VK_FORMAT_D24_UNORM_S8_UINT,
		};

		return vkFormats[static_cast<uint8>(imageFormat)];
	}

	static ImageFormat getImageFormat(VkFormat vkFormat) {
		if (vkFormat == VK_FORMAT_R8_UNORM) {
			return ImageFormat::R8_UNORM;
		}

		if (vkFormat == VK_FORMAT_B8G8R8A8_UNORM) {
			return ImageFormat::B8G8R8A8_UNORM;
		}

		if (vkFormat == VK_FORMAT_R8G8B8A8_SRGB) {
			return ImageFormat::R8G8B8A8_SRGB;
		}

		if (vkFormat == VK_FORMAT_R8G8B8A8_UNORM) {
			return ImageFormat::R8G8B8A8_UNORM;
		}

		if (vkFormat == VK_FORMAT_D32_SFLOAT) {
			return ImageFormat::D32_SFLOAT;
		}

		if (vkFormat == VK_FORMAT_D32_SFLOAT_S8_UINT) {
			return ImageFormat::D32_SFLOAT_S8_UINT;
		}

		if (vkFormat == VK_FORMAT_D24_UNORM_S8_UINT) {
			return ImageFormat::D24_UNORM_S8_UINT;
		}

		DEBUG_ASSERT(false);

		return ImageFormat::R8_UNORM;
	}
}