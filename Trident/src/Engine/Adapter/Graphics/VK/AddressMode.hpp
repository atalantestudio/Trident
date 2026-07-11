// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/AddressMode.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace Trident::VK {
	inline VkSamplerAddressMode getAddressMode(AddressMode addressMode) {
		static constexpr VkSamplerAddressMode ADDRESS_MODES[] = {
			VK_SAMPLER_ADDRESS_MODE_REPEAT,
			VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT,
			VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
			VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER,
		};

		ADAPTER_ASSERT(addressMode < AddressMode::count, "Invalid address mode.");

		return ADDRESS_MODES[static_cast<uint32>(addressMode)];
	}
}