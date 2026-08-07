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
			DXGI_FORMAT_R32G32_UINT,
			DXGI_FORMAT_R32G32_FLOAT,
			DXGI_FORMAT_R32G32B32_UINT,
			DXGI_FORMAT_R32G32B32_FLOAT,
			DXGI_FORMAT_R32G32B32A32_UINT,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			DXGI_FORMAT_UNKNOWN,
			DXGI_FORMAT_UNKNOWN,
			DXGI_FORMAT_UNKNOWN,
			DXGI_FORMAT_UNKNOWN,
			DXGI_FORMAT_UNKNOWN,
			DXGI_FORMAT_UNKNOWN,
		};

		ADAPTER_ASSERT(format < VertexInputAttributeFormat::count, "Invalid format.");

		const DXGI_FORMAT dxFormat = FORMATS[static_cast<uint32>(format)];

		ADAPTER_ASSERT(dxFormat != DXGI_FORMAT_UNKNOWN, "Unsupported format.");

		return dxFormat;
	}
}