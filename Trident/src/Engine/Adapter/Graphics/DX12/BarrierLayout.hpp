// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/GraphicsAdapter.hpp"
#include "Engine/API/Graphics/Access.hpp"
#include "Engine/API/Graphics/ImageLayout.hpp"

namespace Trident::DX12 {
	inline D3D12_BARRIER_LAYOUT getBarrierLayout(ImageLayout imageLayout, Access access) {
		static constexpr D3D12_BARRIER_LAYOUT BARRIER_LAYOUTS[] = {
			D3D12_BARRIER_LAYOUT_COMMON,
			D3D12_BARRIER_LAYOUT_UNDEFINED,
			D3D12_BARRIER_LAYOUT_RENDER_TARGET,
			D3D12_BARRIER_LAYOUT_DEPTH_STENCIL_WRITE,
			D3D12_BARRIER_LAYOUT_DEPTH_STENCIL_READ,
			D3D12_BARRIER_LAYOUT_SHADER_RESOURCE,
			D3D12_BARRIER_LAYOUT_COPY_SOURCE,
			D3D12_BARRIER_LAYOUT_COPY_DEST,
			D3D12_BARRIER_LAYOUT_PRESENT,
		};

		ADAPTER_ASSERT(imageLayout < ImageLayout::count, "Invalid image layout.");

		if (imageLayout == ImageLayout::GENERAL) {
			if (access == Access::SHADER_WRITE) {
				return D3D12_BARRIER_LAYOUT_UNORDERED_ACCESS;
			}

			return D3D12_BARRIER_LAYOUT_SHADER_RESOURCE;
		}

		return BARRIER_LAYOUTS[static_cast<uint32>(imageLayout)];
	}
}