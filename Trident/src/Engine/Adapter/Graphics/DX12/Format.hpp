// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/GraphicsAdapter.hpp"
#include "Engine/API/Rendering/ImageFormat.hpp"

namespace Trident::DX12 {
	inline DXGI_FORMAT getImageFormat(ImageFormat imageFormat) {
		static constexpr DXGI_FORMAT FORMATS[] = {
			DXGI_FORMAT_R8_UNORM,
			DXGI_FORMAT_UNKNOWN,
			DXGI_FORMAT_B8G8R8A8_UNORM,
			DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
			DXGI_FORMAT_R8G8B8A8_UNORM,

			DXGI_FORMAT_R32_UINT,
			DXGI_FORMAT_R32G32_UINT,

			DXGI_FORMAT_D32_FLOAT,
			DXGI_FORMAT_D32_FLOAT_S8X24_UINT,
			DXGI_FORMAT_D24_UNORM_S8_UINT,
		};

		ADAPTER_ASSERT(imageFormat < ImageFormat::count, "Invalid image format.");
		ADAPTER_ASSERT(imageFormat != ImageFormat::R8G8B8_UNORM, "ImageFormat::R8G8B8_UNORM is not supported.");

		return FORMATS[static_cast<uint32>(imageFormat)];
	}

	inline DXGI_FORMAT getIndexBufferFormat(uint8 indexTypeSize) {
		switch (indexTypeSize) {
			case sizeof(uint16): {
				return DXGI_FORMAT_R16_UINT;
			}
			case sizeof(uint32): {
				return DXGI_FORMAT_R32_UINT;
			}
			default: {
				ADAPTER_ASSERT(false, "Unsupported index type size.");

				return DXGI_FORMAT_UNKNOWN;
			}
		}
	}
}