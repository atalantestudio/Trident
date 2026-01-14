// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/GraphicsAdapter.hpp"
#include "Engine/API/Rendering/ImageType.hpp"

namespace ProjectA::DX12 {
	inline D3D12_RESOURCE_DIMENSION getImageResourceDimension(ImageType imageType) {
		switch (imageType) {
			case ImageType::TEXTURE_2D: {
				return D3D12_RESOURCE_DIMENSION_TEXTURE2D;
			}
			default: {
				ADAPTER_ASSERT(false, "Unsupported image type.");

				return D3D12_RESOURCE_DIMENSION_UNKNOWN;
			}
		}
	}
}