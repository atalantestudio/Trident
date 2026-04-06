// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/GraphicsAdapter.hpp"
#include "Engine/API/Graphics/PrimitiveTopology.hpp"

namespace Trident::DX12 {
	inline D3D12_PRIMITIVE_TOPOLOGY_TYPE getPrimitiveTopologyType(PrimitiveTopology primitiveTopology) {
		static constexpr D3D12_PRIMITIVE_TOPOLOGY_TYPE PRIMITIVE_TOPOLOGY_TYPES[] = {
			D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT,
			D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE,
			D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE,
			D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
			D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
		};

		ADAPTER_ASSERT(primitiveTopology < PrimitiveTopology::count, "Invalid primitive topology.");

		return PRIMITIVE_TOPOLOGY_TYPES[static_cast<uint32>(primitiveTopology)];
	}
}