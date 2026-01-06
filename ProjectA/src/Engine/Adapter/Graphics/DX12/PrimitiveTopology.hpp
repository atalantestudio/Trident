// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/GraphicsAdapter.hpp"
#include "Engine/API/Rendering/PrimitiveTopology.hpp"

namespace ProjectA::DX12 {
	D3D_PRIMITIVE_TOPOLOGY getPrimitiveTopology(PrimitiveTopology primitiveTopology) {
		switch (primitiveTopology) {
			case PrimitiveTopology::POINT_LIST: return D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
			case PrimitiveTopology::LINE_LIST: return D3D_PRIMITIVE_TOPOLOGY_LINELIST;
			case PrimitiveTopology::LINE_STRIP: return D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
			case PrimitiveTopology::TRIANGLE_LIST: return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			case PrimitiveTopology::TRIANGLE_STRIP: return D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
			default: {
				ADAPTER_ASSERT(false, "Undefined primitive topology.");

				return D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
			}
		}
	}
}