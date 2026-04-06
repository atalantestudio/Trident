// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/GraphicsAdapter.hpp"
#include "Engine/API/Graphics/BlendFactor.hpp"

namespace Trident::DX12 {
	inline D3D12_BLEND getBlendFactor(BlendFactor blendFactor) {
		static constexpr D3D12_BLEND BLEND_FACTORS[] = {
			D3D12_BLEND_ZERO,
			D3D12_BLEND_ONE,
			D3D12_BLEND_SRC_COLOR,
			D3D12_BLEND_INV_SRC_COLOR,
			D3D12_BLEND_DEST_COLOR,
			D3D12_BLEND_INV_DEST_COLOR,
			D3D12_BLEND_SRC_ALPHA,
			D3D12_BLEND_INV_SRC_ALPHA,
			D3D12_BLEND_DEST_ALPHA,
			D3D12_BLEND_INV_DEST_ALPHA,
			D3D12_BLEND_BLEND_FACTOR,
			D3D12_BLEND_INV_BLEND_FACTOR,
			D3D12_BLEND_ALPHA_FACTOR,
			D3D12_BLEND_INV_ALPHA_FACTOR,
			D3D12_BLEND_SRC_ALPHA_SAT,
			D3D12_BLEND_SRC1_COLOR,
			D3D12_BLEND_INV_SRC1_COLOR,
			D3D12_BLEND_SRC1_ALPHA,
			D3D12_BLEND_INV_SRC1_ALPHA,
		};

		ADAPTER_ASSERT(blendFactor < BlendFactor::count, "Invalid blend factor.");

		return BLEND_FACTORS[static_cast<uint32>(blendFactor)];
	}
}