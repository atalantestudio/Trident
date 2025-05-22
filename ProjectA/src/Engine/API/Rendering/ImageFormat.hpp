#pragma once

namespace ProjectA {
	enum class ImageFormat : uint8 {
		R8_UNORM = 0,
		B8G8R8A8_UNORM = 1,
		R8G8B8A8_SRGB = 2,
		R8G8B8A8_UNORM = 3,
		D32_SFLOAT = 4,
		D32_SFLOAT_S8_UINT = 5,
		D24_UNORM_S8_UINT = 6,
	};
}