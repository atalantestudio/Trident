// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/AddressMode.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace Trident::GL {
	inline GLenum getAddressMode(AddressMode addressMode) {
		static constexpr GLenum ADDRESS_MODES[] = {
			GL_REPEAT,
			GL_MIRRORED_REPEAT,
			GL_CLAMP_TO_EDGE,
			GL_CLAMP_TO_BORDER,
		};

		ADAPTER_ASSERT(addressMode < AddressMode::count, "Invalid address mode.");

		return ADDRESS_MODES[static_cast<uint32>(addressMode)];
	}
}