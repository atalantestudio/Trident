// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/GraphicsAdapter.hpp"
#include "Engine/API/Rendering/BlendFactor.hpp"

namespace Trident::DX12 {
	D3D12_BLEND getBlendFactor(BlendFactor blendFactor) {
		switch (blendFactor) {
			case BlendFactor::ZERO: return D3D12_BLEND_ZERO;
			case BlendFactor::ONE: return D3D12_BLEND_ONE;
			case BlendFactor::SRC_COLOR: return D3D12_BLEND_SRC_COLOR;
			case BlendFactor::ONE_MINUS_SRC_COLOR: return D3D12_BLEND_INV_SRC_COLOR;
			case BlendFactor::DST_COLOR: return D3D12_BLEND_DEST_COLOR;
			case BlendFactor::ONE_MINUS_DST_COLOR: return D3D12_BLEND_INV_DEST_COLOR;
			case BlendFactor::SRC_ALPHA: return D3D12_BLEND_SRC_ALPHA;
			case BlendFactor::ONE_MINUS_SRC_ALPHA: return D3D12_BLEND_INV_SRC_ALPHA;
			case BlendFactor::DST_ALPHA: return D3D12_BLEND_DEST_ALPHA;
			case BlendFactor::ONE_MINUS_DST_ALPHA: return D3D12_BLEND_INV_DEST_ALPHA;
			case BlendFactor::CONSTANT_COLOR: return D3D12_BLEND_BLEND_FACTOR;
			case BlendFactor::ONE_MINUS_CONSTANT_COLOR: return D3D12_BLEND_INV_BLEND_FACTOR;
			case BlendFactor::CONSTANT_ALPHA: return D3D12_BLEND_ALPHA_FACTOR;
			case BlendFactor::ONE_MINUS_CONSTANT_ALPHA: return D3D12_BLEND_INV_ALPHA_FACTOR;
			case BlendFactor::SRC_ALPHA_SATURATE: return D3D12_BLEND_SRC_ALPHA_SAT;
			case BlendFactor::SRC1_COLOR: return D3D12_BLEND_SRC1_COLOR;
			case BlendFactor::ONE_MINUS_SRC1_COLOR: return D3D12_BLEND_INV_SRC1_COLOR;
			case BlendFactor::SRC1_ALPHA: return D3D12_BLEND_SRC1_ALPHA;
			case BlendFactor::ONE_MINUS_SRC1_ALPHA: return D3D12_BLEND_INV_SRC1_ALPHA;
			default: {
				ADAPTER_ASSERT(false, "Undefined blend factor.");

				return D3D12_BLEND_ZERO;
			}
		}
	}
}