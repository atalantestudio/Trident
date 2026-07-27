// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	enum class ImageFormat : uint8 {
		// Fixed/floating point color formats.
		R8_UNORM,

		// NOTE: Only used for asset storage.
		R8G8B8_UNORM,

		B8G8R8A8_UNORM,
		R8G8B8A8_SRGB,
		R8G8B8A8_UNORM,

		R32G32B32A32_SFLOAT,

		// Unsigned integer formats.
		R32_UINT,
		R32G32_UINT,

		// Depth/depth-stencil formats.
		D32_SFLOAT,
		D32_SFLOAT_S8_UINT,
		D24_UNORM_S8_UINT,

		count,
	};

	inline bool isDepthFormat(ImageFormat imageFormat) {
		static constexpr ImageFormat FIRST_DEPTH_FORMAT = ImageFormat::D32_SFLOAT;

		DEBUG_ASSERT(imageFormat < ImageFormat::count);

		return imageFormat >= FIRST_DEPTH_FORMAT;
	}

	inline bool isStencilFormat(ImageFormat imageFormat) {
		static constexpr ImageFormat FIRST_STENCIL_FORMAT = ImageFormat::D32_SFLOAT_S8_UINT;

		DEBUG_ASSERT(imageFormat < ImageFormat::count);

		return imageFormat >= FIRST_STENCIL_FORMAT;
	}

	inline bool isFixedOrFloatingPointColorFormat(ImageFormat imageFormat) {
		static constexpr ImageFormat LAST_FIXED_OR_FLOATING_POINT_COLOR_FORMAT = ImageFormat::R32G32B32A32_SFLOAT;

		DEBUG_ASSERT(imageFormat < ImageFormat::count);

		return imageFormat <= LAST_FIXED_OR_FLOATING_POINT_COLOR_FORMAT;
	}

	inline bool isSignedIntegerColorFormat(ImageFormat imageFormat) {
		return false;
	}

	inline bool isUnsignedIntegerColorFormat(ImageFormat imageFormat) {
		static constexpr ImageFormat FIRST_UNSIGNED_INTEGER_FORMAT = ImageFormat::R32_UINT;
		static constexpr ImageFormat LAST_UNSIGNED_INTEGER_FORMAT = ImageFormat::R32G32_UINT;

		DEBUG_ASSERT(imageFormat < ImageFormat::count);

		return imageFormat >= FIRST_UNSIGNED_INTEGER_FORMAT && imageFormat <= LAST_UNSIGNED_INTEGER_FORMAT;
	}

	inline ImageFormat findUnormFormat(uint8 channelCount, uint8 channelBitCount) {
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

	inline uint8 getByteSize(ImageFormat imageFormat) {
		static constexpr uint8 BYTE_SIZES[] = {
			sizeof(uint8),
			sizeof(uint8) * 3,
			sizeof(uint8) * 4,
			sizeof(uint8) * 4,
			sizeof(uint8) * 4,
			sizeof(float32) * 4,
			sizeof(uint32),
			sizeof(uint32) * 2,
			sizeof(float32),
			sizeof(float32) + sizeof(uint8), // D32_SFLOAT_S8_UINT
			sizeof(uint32), // D24_UNORM_S8_UINT
		};

		ENGINE_ASSERT(imageFormat < ImageFormat::count, "Invalid image format.");

		return BYTE_SIZES[static_cast<uint32>(imageFormat)];
	}

	inline uint8 getChannelCount(ImageFormat imageFormat) {
		static constexpr uint8 CHANNEL_COUNTS[] = {
			1,
			3,
			4,
			4,
			4,
			4,
			1,
			2,
			1,
		};

		ENGINE_ASSERT(imageFormat < ImageFormat::D32_SFLOAT_S8_UINT, "Invalid image format");

		return CHANNEL_COUNTS[static_cast<uint32>(imageFormat)];
	}
}