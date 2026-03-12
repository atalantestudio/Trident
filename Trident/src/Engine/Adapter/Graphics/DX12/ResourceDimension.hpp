// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/GraphicsAdapter.hpp"
#include "Engine/API/Rendering/ImageType.hpp"

namespace Trident::DX12 {
	inline D3D12_RESOURCE_DIMENSION getImageResourceDimension(ImageType imageType) {
		static constexpr D3D12_RESOURCE_DIMENSION RESOURCE_DIMENSIONS[] = {
			D3D12_RESOURCE_DIMENSION_TEXTURE2D,
		};

		ADAPTER_ASSERT(imageType < ImageType::count, "Invalid image type.");

		return RESOURCE_DIMENSIONS[static_cast<uint32>(imageType)];
	}

	inline D3D12_RTV_DIMENSION getRenderTargetViewDimension(ImageType imageType, uint32 layerCount) {
		static constexpr D3D12_RTV_DIMENSION RENDER_TARGET_VIEW_DIMENSIONS[] = {
			D3D12_RTV_DIMENSION_TEXTURE2D,
		};

		ADAPTER_ASSERT(imageType < ImageType::count, "Invalid image type.");

		const D3D12_RTV_DIMENSION renderTargetViewDimension = RENDER_TARGET_VIEW_DIMENSIONS[static_cast<uint32>(imageType)];

		if (layerCount > 1) {
			return static_cast<D3D12_RTV_DIMENSION>(static_cast<uint32>(renderTargetViewDimension) + 1);
		}

		return renderTargetViewDimension;
	}

	inline D3D12_DSV_DIMENSION getDepthStencilViewDimension(ImageType imageType, uint32 layerCount) {
		static constexpr D3D12_DSV_DIMENSION DEPTH_STENCIL_VIEW_DIMENSIONS[] = {
			D3D12_DSV_DIMENSION_TEXTURE2D,
		};

		ADAPTER_ASSERT(imageType < ImageType::count, "Invalid image type.");

		const D3D12_DSV_DIMENSION depthStencilViewDimension = DEPTH_STENCIL_VIEW_DIMENSIONS[static_cast<uint32>(imageType)];

		if (layerCount > 1) {
			return static_cast<D3D12_DSV_DIMENSION>(static_cast<uint32>(depthStencilViewDimension) + 1);
		}

		return depthStencilViewDimension;
	}
}