// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/GraphicsAdapter.hpp"
#include "Engine/API/Rendering/CullMode.hpp"

namespace Trident::DX12 {
	inline D3D12_CULL_MODE getCullMode(CullMode cullMode) {
		static constexpr D3D12_CULL_MODE CULL_MODES[] = {
			D3D12_CULL_MODE_NONE,
			D3D12_CULL_MODE_FRONT,
			D3D12_CULL_MODE_BACK,
		};

		ADAPTER_ASSERT(cullMode < CullMode::count, "Invalid cull mode.");

		return CULL_MODES[static_cast<uint32>(cullMode)];
	}
}