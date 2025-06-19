#pragma once

namespace ProjectA {
	enum class ImageFormat : uint8 {
		R8_UNORM = 0,
		R8G8B8_UNORM = 1,
		B8G8R8A8_UNORM = 2,
		R8G8B8A8_SRGB = 3,
		R8G8B8A8_UNORM = 4,
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
}