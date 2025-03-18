#pragma once

#include <vulkan/vulkan.h>

#include "Engine/Renderer/CullMode.hpp"

namespace ProjectA {
	static VkCullModeFlagBits getVkCullMode(CullMode cullMode) {
		static constexpr VkCullModeFlagBits vkCullModes[] = {
			VK_CULL_MODE_NONE,
			VK_CULL_MODE_FRONT_BIT,
			VK_CULL_MODE_BACK_BIT,
			VK_CULL_MODE_FRONT_AND_BACK,
		};

		return vkCullModes[static_cast<uint32>(cullMode)];
	}
}