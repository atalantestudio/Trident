// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/GraphicsAdapter.hpp"
#include "Engine/API/Graphics/IndexFormat.hpp"

namespace Trident::DX12 {
	inline DXGI_FORMAT translateIndexFormat(IndexFormat format) {
		switch (format) {
			case R16_UINT: {
				return DXGI_FORMAT_R16_UINT;
			}
			case R32_UINT: {
				return DXGI_FORMAT_R32_UINT;
			}
			default: {
				ADAPTER_ASSERT(false, "Unsupported index format.");

				return DXGI_FORMAT_UNKNOWN;
			}
		}
	}
}