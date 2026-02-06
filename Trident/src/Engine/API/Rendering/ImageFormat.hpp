// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	enum class ImageFormat : uint8 {
		// Fixed/floating point color formats.
		R8_UNORM,
		R8G8B8_UNORM,
		B8G8R8A8_UNORM,
		R8G8B8A8_SRGB,
		R8G8B8A8_UNORM,

		// Unsigned integer formats.
		R32_UINT,
		R32G32_UINT,

		// Depth/depth-stencil formats.
		D32_SFLOAT,
		D32_SFLOAT_S8_UINT,
		D24_UNORM_S8_UINT,

		count,
	};

	static bool isDepthFormat(ImageFormat imageFormat) {
		static constexpr ImageFormat FIRST_DEPTH_FORMAT = ImageFormat::D32_SFLOAT;

		DEBUG_ASSERT(imageFormat < ImageFormat::count);

		return imageFormat >= FIRST_DEPTH_FORMAT;
	}

	static bool isFixedOrFloatingPointColorFormat(ImageFormat imageFormat) {
		static constexpr ImageFormat LAST_FIXED_OR_FLOATING_POINT_COLOR_FORMAT = ImageFormat::R8G8B8A8_UNORM;

		DEBUG_ASSERT(imageFormat < ImageFormat::count);

		return imageFormat <= LAST_FIXED_OR_FLOATING_POINT_COLOR_FORMAT;
	}

	static bool isSignedIntegerColorFormat(ImageFormat imageFormat) {
		return false;
	}

	static bool isUnsignedIntegerColorFormat(ImageFormat imageFormat) {
		static constexpr ImageFormat FIRST_UNSIGNED_INTEGER_FORMAT = ImageFormat::R32_UINT;
		static constexpr ImageFormat LAST_UNSIGNED_INTEGER_FORMAT = ImageFormat::R32G32_UINT;

		DEBUG_ASSERT(imageFormat < ImageFormat::count);

		return imageFormat <= FIRST_UNSIGNED_INTEGER_FORMAT && imageFormat >= LAST_UNSIGNED_INTEGER_FORMAT;
	}

	static ImageFormat findUnormFormat(uint8 channelCount, uint8 channelBitCount) {
		ENGINE_ASSERT(channelBitCount == 8, "Unsupported channel bit count.");

		switch (channelCount) {
			case 1: return ImageFormat::R8_UNORM;
			case 3: return ImageFormat::R8G8B8_UNORM;
			case 4: return ImageFormat::R8G8B8A8_UNORM;
			default: {
				ENGINE_ASSERT(false, "Unsupported channel count for *UNORM format.");

				return static_cast<ImageFormat>(0);
			}
		}
	}

	static uint8 getChannelCount(ImageFormat imageFormat) {
		switch (imageFormat) {
			case ImageFormat::R8_UNORM:
			case ImageFormat::R32_UINT: {
				return 1;
			}
			case ImageFormat::R32G32_UINT: {
				return 2;
			}
			case ImageFormat::R8G8B8_UNORM: {
				return 3;
			}
			case ImageFormat::B8G8R8A8_UNORM:
			case ImageFormat::R8G8B8A8_SRGB:
			case ImageFormat::R8G8B8A8_UNORM: {
				return 4;
			}
			default:
				ENGINE_ASSERT(false, "Unsupported image format.");

				return 0;
		}
	}
}