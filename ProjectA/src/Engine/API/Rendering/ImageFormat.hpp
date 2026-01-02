#pragma once

namespace ProjectA {
	enum class ImageFormat : uint8 {
		// Color formats.
		R8_UNORM,
		R8G8B8_UNORM,
		B8G8R8A8_UNORM,
		R8G8B8A8_SRGB,
		R8G8B8A8_UNORM,
		R32_UINT,
		R32G32_UINT,

		// Depth/depth-stencil formats.
		D32_SFLOAT,
		D32_SFLOAT_S8_UINT,
		D24_UNORM_S8_UINT,

		count,
	};

	static bool isDepthFormat(ImageFormat imageFormat) {
		return (
			imageFormat == ImageFormat::D32_SFLOAT ||
			imageFormat == ImageFormat::D32_SFLOAT_S8_UINT ||
			imageFormat == ImageFormat::D24_UNORM_S8_UINT
		);
	}

	static bool isFixedOrFloatingPointColorFormat(ImageFormat imageFormat) {
		return (
			imageFormat == ImageFormat::R8_UNORM ||
			imageFormat == ImageFormat::R8G8B8_UNORM ||
			imageFormat == ImageFormat::B8G8R8A8_UNORM ||
			imageFormat == ImageFormat::R8G8B8A8_SRGB ||
			imageFormat == ImageFormat::R8G8B8A8_UNORM
		);
	}

	static bool isSignedIntegerColorFormat(ImageFormat imageFormat) {
		return false;
	}

	static bool isUnsignedIntegerColorFormat(ImageFormat imageFormat) {
		return (
			imageFormat == ImageFormat::R32_UINT ||
			imageFormat == ImageFormat::R32G32_UINT
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