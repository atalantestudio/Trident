#pragma once

#include "Engine/Renderer/TextureUsage.hpp"

namespace ProjectA {
	static VkImageUsageFlagBits getVkImageUsageFlagBits(TextureUsage textureUsage) {
		return static_cast<VkImageUsageFlagBits>(textureUsage);
	}
}