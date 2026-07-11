// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/AddressMode.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace Trident::DX12 {
	inline D3D12_TEXTURE_ADDRESS_MODE getAddressMode(AddressMode addressMode) {
		static constexpr D3D12_TEXTURE_ADDRESS_MODE ADDRESS_MODES[] = {
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,
			D3D12_TEXTURE_ADDRESS_MODE_MIRROR,
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
			D3D12_TEXTURE_ADDRESS_MODE_BORDER,
		};

		ADAPTER_ASSERT(addressMode < AddressMode::count, "Invalid address mode.");

		return ADDRESS_MODES[static_cast<uint32>(addressMode)];
	}
}