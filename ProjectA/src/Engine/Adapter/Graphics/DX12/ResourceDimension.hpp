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

	inline D3D12_RTV_DIMENSION getRenderTargetViewDimension(ImageType imageType, uint32 layerCount) {
		switch (imageType) {
			case ImageType::TEXTURE_2D: {
				if (layerCount > 1) {
					return D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
				}

				return D3D12_RTV_DIMENSION_TEXTURE2D;
			}
			default: {
				ADAPTER_ASSERT(false, "Unsupported image type.");

				return D3D12_RTV_DIMENSION_UNKNOWN;
			}
		}
	}

	inline D3D12_DSV_DIMENSION getDepthStencilViewDimension(ImageType imageType, uint32 layerCount) {
		switch (imageType) {
			case ImageType::TEXTURE_2D: {
				if (layerCount > 1) {
					return D3D12_DSV_DIMENSION_TEXTURE2DARRAY;
				}

				return D3D12_DSV_DIMENSION_TEXTURE2D;
			}
			default: {
				ADAPTER_ASSERT(false, "Unsupported image type.");

				return D3D12_DSV_DIMENSION_UNKNOWN;
			}
		}
	}
}