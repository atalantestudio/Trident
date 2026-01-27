// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/CullMode.hpp"

namespace Trident::VK {
	inline VkCullModeFlagBits getCullMode(CullMode cullMode) {
		static constexpr VkCullModeFlagBits vkCullModes[] = {
			VK_CULL_MODE_NONE,
			VK_CULL_MODE_FRONT_BIT,
			VK_CULL_MODE_BACK_BIT,
			VK_CULL_MODE_FRONT_AND_BACK,
		};

		return vkCullModes[static_cast<uint8>(cullMode)];
	}
}