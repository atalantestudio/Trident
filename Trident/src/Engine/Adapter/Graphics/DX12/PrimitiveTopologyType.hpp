// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/GraphicsAdapter.hpp"
#include "Engine/API/Rendering/PrimitiveTopology.hpp"

namespace Trident::DX12 {
	D3D12_PRIMITIVE_TOPOLOGY_TYPE getPrimitiveTopologyType(PrimitiveTopology primitiveTopology) {
		switch (primitiveTopology) {
			case PrimitiveTopology::POINT_LIST: return D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
			case PrimitiveTopology::LINE_LIST: return D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
			case PrimitiveTopology::LINE_STRIP: {
				ADAPTER_ASSERT(false, "LINE_STRIP primitive topology is not supported.");

				return D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED;
			}
			case PrimitiveTopology::TRIANGLE_LIST: {
				return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			}
			case PrimitiveTopology::TRIANGLE_STRIP: {
				ADAPTER_ASSERT(false, "TRIANGLE_STRIP primitive topology is not supported.");

				return D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED;
			}
			default: {
				ADAPTER_ASSERT(false, "Undefined primitive topology.");

				return D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED;
			}
		}
	}
}