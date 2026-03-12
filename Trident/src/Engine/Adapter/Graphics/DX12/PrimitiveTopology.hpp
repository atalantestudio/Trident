// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/GraphicsAdapter.hpp"
#include "Engine/API/Rendering/PrimitiveTopology.hpp"

namespace Trident::DX12 {
	inline D3D_PRIMITIVE_TOPOLOGY getPrimitiveTopology(PrimitiveTopology primitiveTopology) {
		static constexpr D3D_PRIMITIVE_TOPOLOGY PRIMITIVE_TOPOLOGIES[] = {
			D3D_PRIMITIVE_TOPOLOGY_POINTLIST,
			D3D_PRIMITIVE_TOPOLOGY_LINELIST,
			D3D_PRIMITIVE_TOPOLOGY_LINESTRIP,
			D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
			D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
		};

		ADAPTER_ASSERT(primitiveTopology < PrimitiveTopology::count, "Invalid primitive topology.");

		return PRIMITIVE_TOPOLOGIES[static_cast<uint32>(primitiveTopology)];
	}
}