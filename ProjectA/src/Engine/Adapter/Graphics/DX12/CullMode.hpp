// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/GraphicsAdapter.hpp"
#include "Engine/API/Rendering/CullMode.hpp"

namespace ProjectA::DX12 {
	D3D12_CULL_MODE getCullMode(CullMode cullMode) {
		switch (cullMode) {
			case CullMode::NONE:
				return D3D12_CULL_MODE_NONE;
			case CullMode::FRONT:
				return D3D12_CULL_MODE_FRONT;
			case CullMode::BACK:
				return D3D12_CULL_MODE_BACK;
			case CullMode::FRONT_AND_BACK:
				ADAPTER_ASSERT(false, "FRONT_AND_BACK cull mode is not supported.");

				return D3D12_CULL_MODE_NONE;
			default:
				ADAPTER_ASSERT(false, "Undefined cull mode.");

				return D3D12_CULL_MODE_NONE;
		}
	}
}