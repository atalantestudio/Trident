// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/GraphicsAdapter.hpp"
#include "Engine/API/Graphics/VertexInputAttributeFormat.hpp"

namespace Trident::DX12 {
	inline DXGI_FORMAT getVertexInputAttributeFormat(VertexInputAttributeFormat format) {
		static constexpr DXGI_FORMAT FORMATS[static_cast<uint32>(VertexInputAttributeFormat::count)] = {
			DXGI_FORMAT_R8_SINT,
			DXGI_FORMAT_R8_UINT,
			DXGI_FORMAT_R10G10B10A2_UNORM,
			DXGI_FORMAT_R16_SINT,
			DXGI_FORMAT_R16_UINT,
			DXGI_FORMAT_R32_SINT,
			DXGI_FORMAT_R32_UINT,
			DXGI_FORMAT_R32_FLOAT,
			DXGI_FORMAT_R16G16_UNORM,
			DXGI_FORMAT_R16G16_FLOAT,
			DXGI_FORMAT_R32G32_SINT,
			DXGI_FORMAT_R32G32_UINT,
			DXGI_FORMAT_R32G32_FLOAT,
			DXGI_FORMAT_R32G32B32_SINT,
			DXGI_FORMAT_R32G32B32_UINT,
			DXGI_FORMAT_R32G32B32_FLOAT,
			DXGI_FORMAT_R32G32B32A32_SINT,
			DXGI_FORMAT_R32G32B32A32_UINT,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
		};

		ADAPTER_ASSERT(format < VertexInputAttributeFormat::count, "Invalid format.");

		return FORMATS[static_cast<uint32>(format)];
	}
}