// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/GraphicsAdapter.hpp"
#include "Engine/API/Rendering/ImageFormat.hpp"

namespace ProjectA::DX12 {
	inline DXGI_FORMAT getImageFormat(ImageFormat imageFormat) {
		switch (imageFormat) {
			case ImageFormat::R8_UNORM:
				return DXGI_FORMAT_R8_UNORM;
			case ImageFormat::R8G8B8_UNORM:
				ADAPTER_ASSERT(false, "R8G8B8_UNORM format is not supported.");

				return DXGI_FORMAT_UNKNOWN;
			case ImageFormat::B8G8R8A8_UNORM:
				return DXGI_FORMAT_B8G8R8A8_UNORM;
			case ImageFormat::R8G8B8A8_SRGB:
				// TODO: Check if this is the right format.
				return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
			case ImageFormat::R8G8B8A8_UNORM:
				return DXGI_FORMAT_R8G8B8A8_UNORM;
			case ImageFormat::R32_UINT:
				return DXGI_FORMAT_R32_UINT;
			case ImageFormat::R32G32_UINT:
				return DXGI_FORMAT_R32G32_UINT;
			case ImageFormat::D32_SFLOAT:
				// TODO: Check if this is the right format.
				return DXGI_FORMAT_D32_FLOAT;
			case ImageFormat::D32_SFLOAT_S8_UINT:
				// TODO: Check if this is the right format.
				return DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
			case ImageFormat::D24_UNORM_S8_UINT:
				return DXGI_FORMAT_D24_UNORM_S8_UINT;
			default:
				ADAPTER_ASSERT(false, "Undefined format.");

				return DXGI_FORMAT_UNKNOWN;
		}
	}

	inline DXGI_FORMAT getIndexBufferFormat(uint8 indexTypeSize) {
		switch (indexTypeSize) {
			case sizeof(uint16): return DXGI_FORMAT_R16_UINT;
			case sizeof(uint32): return DXGI_FORMAT_R32_UINT;
			default: {
				ADAPTER_ASSERT(false, "Unsupported index type size.");

				return DXGI_FORMAT_UNKNOWN;
			}
		}
	}
}