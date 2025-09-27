#pragma once

namespace ProjectA {
	enum class ImageFormat : uint8 {
		// Color formats.
		R8_UNORM = 0,
		R8G8B8_UNORM = 1,
		B8G8R8A8_UNORM = 2,
		R8G8B8A8_SRGB = 3,
		R8G8B8A8_UNORM = 4,
		R32G32_UINT = 5,

		// Depth/depth-stencil formats.
		D32_SFLOAT = 6,
		D32_SFLOAT_S8_UINT = 7,
		D24_UNORM_S8_UINT = 8,

		__MAX_ENUM,
	};

	enum class DepthStencilImageFormat : uint8 {
		D32_SFLOAT = 6,
		D32_SFLOAT_S8_UINT = 7,
		D24_UNORM_S8_UINT = 8,
	};

	static bool isDepthFormat(ImageFormat imageFormat) {
		return (
			imageFormat == ImageFormat::D32_SFLOAT ||
			imageFormat == ImageFormat::D32_SFLOAT_S8_UINT ||
			imageFormat == ImageFormat::D24_UNORM_S8_UINT
		);
	}

	static ImageFormat findUnormFormat(uint8 channelCount) {
		switch (channelCount) {
			case 1:
				return ImageFormat::R8_UNORM;
			case 3:
				return ImageFormat::R8G8B8_UNORM;
			case 4:
				return ImageFormat::R8G8B8A8_UNORM;
			default:
				ENGINE_ASSERT(false, "Unsupported channel count for *UNORM format.");

				return static_cast<ImageFormat>(0);
		}
	}

	static uint8 getChannelCount(ImageFormat imageFormat) {
		switch (imageFormat) {
			case ImageFormat::R8_UNORM:
				return 1;
			case ImageFormat::R8G8B8_UNORM:
				return 3;
			case ImageFormat::B8G8R8A8_UNORM:
			case ImageFormat::R8G8B8A8_SRGB:
			case ImageFormat::R8G8B8A8_UNORM:
				return 4;
			case ImageFormat::R32G32_UINT:
				return 2;
			default:
				ENGINE_ASSERT(false, "Unsupported image format.");

				return 0;
		}
	}
}