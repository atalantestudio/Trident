// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/GraphicsAdapter.hpp"
#include "Engine/API/Graphics/IndexFormat.hpp"

namespace Trident::DX12 {
	inline DXGI_FORMAT translateIndexFormat(IndexFormat format) {
		static constexpr DXGI_FORMAT INDEX_FORMATS[] = {
			DXGI_FORMAT_R16_UINT,
			DXGI_FORMAT_R32_UINT,
		};

		ADAPTER_ASSERT(format < IndexFormat::count, "Unsupported index format.");

		return INDEX_FORMATS[static_cast<uint32>(format)];
	}
}