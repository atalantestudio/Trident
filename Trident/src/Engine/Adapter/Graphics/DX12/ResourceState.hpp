// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/GraphicsAdapter.hpp"
#include "Engine/API/Rendering/BufferUsage.hpp"
#include "Engine/API/Rendering/ImageLayout.hpp"

namespace Trident::DX12 {
	inline D3D12_RESOURCE_STATES getInitialResourceState(D3D12_RESOURCE_STATES resourceStates, D3D12_HEAP_TYPE heapType) {
		switch (heapType) {
			case D3D12_HEAP_TYPE_UPLOAD: {
				return D3D12_RESOURCE_STATE_GENERIC_READ;
			}
			case D3D12_HEAP_TYPE_READBACK: {
				return D3D12_RESOURCE_STATE_COPY_DEST;
			}
			default: {
				return resourceStates;
			}
		}
	}

	inline D3D12_RESOURCE_STATES getImageTransitionResourceState(ImageLayout imageLayout) {
		static constexpr D3D12_RESOURCE_STATES RESOURCE_STATES[] = {
			D3D12_RESOURCE_STATE_COMMON,

			// TODO: Should resources in GENERAL layout have D3D12_RESOURCE_STATE_UNORDERED_ACCESS instead?
			D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE,

			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_RENDER_TARGET | D3D12_RESOURCE_STATE_DEPTH_WRITE,
			D3D12_RESOURCE_STATE_DEPTH_READ,
			D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE,
			D3D12_RESOURCE_STATE_COPY_SOURCE,
			D3D12_RESOURCE_STATE_COPY_DEST,
			D3D12_RESOURCE_STATE_PRESENT,
		};

		ADAPTER_ASSERT(imageLayout < ImageLayout::count, "Invalid image layout.");

		return RESOURCE_STATES[static_cast<uint32>(imageLayout)];
	}
}