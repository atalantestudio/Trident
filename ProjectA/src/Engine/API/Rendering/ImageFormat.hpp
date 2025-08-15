#pragma once

namespace ProjectA {
	enum class ImageFormat : uint8 {
		// Color formats.
		R8_UNORM = 0,
		R8G8B8_UNORM = 1,
		B8G8R8A8_UNORM = 2,
		R8G8B8A8_SRGB = 3,
		R8G8B8A8_UNORM = 4,

		// Depth/depth-stencil formats.
		D32_SFLOAT = 5,
		D32_SFLOAT_S8_UINT = 6,
		D24_UNORM_S8_UINT = 7,
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
}