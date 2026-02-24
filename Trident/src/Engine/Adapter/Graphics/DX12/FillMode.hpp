// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/GraphicsAdapter.hpp"
#include "Engine/API/Rendering/PolygonMode.hpp"

namespace Trident::DX12 {
	inline D3D12_FILL_MODE getFillMode(PolygonMode polygonMode) {
		static constexpr D3D12_FILL_MODE FILL_MODES[] = {
			{},
			D3D12_FILL_MODE_WIREFRAME,
			D3D12_FILL_MODE_SOLID,
		};

		ADAPTER_ASSERT(polygonMode < PolygonMode::count, "Invalid polygon mode.");
		ADAPTER_ASSERT(polygonMode != PolygonMode::POINT, "PolygonMode::POINT is not supported.");

		return FILL_MODES[static_cast<uint32>(polygonMode)];
	}
}