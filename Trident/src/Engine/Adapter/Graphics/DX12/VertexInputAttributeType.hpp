// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/GraphicsAdapter.hpp"
#include "Engine/API/Graphics/VertexInputAttributeType.hpp"

namespace Trident::DX12 {
	inline DXGI_FORMAT getVertexInputAttributeType(VertexInputAttributeType vertexInputAttributeType) {
		static constexpr DXGI_FORMAT VERTEX_INPUT_ATTRIBUTE_TYPES[] = {
			DXGI_FORMAT_R8_SINT,
			DXGI_FORMAT_R8_UINT,
			DXGI_FORMAT_R16_SINT,
			DXGI_FORMAT_R16_UINT,
			DXGI_FORMAT_R32_SINT,
			DXGI_FORMAT_R32_UINT,
			DXGI_FORMAT_R32_FLOAT,
			DXGI_FORMAT_R32G32_FLOAT,
			DXGI_FORMAT_R32G32B32_FLOAT,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			DXGI_FORMAT_UNKNOWN,
			DXGI_FORMAT_UNKNOWN,
			DXGI_FORMAT_UNKNOWN,
			DXGI_FORMAT_UNKNOWN,
			DXGI_FORMAT_UNKNOWN,
			DXGI_FORMAT_UNKNOWN,
		};

		ADAPTER_ASSERT(vertexInputAttributeType < VertexInputAttributeType::count, "Invalid vertex attribute type.");

		const DXGI_FORMAT type = VERTEX_INPUT_ATTRIBUTE_TYPES[static_cast<uint32>(vertexInputAttributeType)];

		ADAPTER_ASSERT(type != DXGI_FORMAT_UNKNOWN, "Unsupported vertex attribute type.");

		return type;
	}
}