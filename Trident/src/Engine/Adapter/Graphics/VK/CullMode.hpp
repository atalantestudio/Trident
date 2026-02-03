// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/GraphicsAdapter.hpp"
#include "Engine/API/Rendering/CullMode.hpp"

namespace Trident::VK {
	inline VkCullModeFlagBits getCullMode(CullMode cullMode) {
		static constexpr VkCullModeFlagBits CULL_MODES[] = {
			VK_CULL_MODE_NONE,
			VK_CULL_MODE_FRONT_BIT,
			VK_CULL_MODE_BACK_BIT,
		};

		ADAPTER_ASSERT(cullMode < CullMode::count, "Invalid cull mode.");		

		return CULL_MODES[static_cast<uint8>(cullMode)];
	}
}