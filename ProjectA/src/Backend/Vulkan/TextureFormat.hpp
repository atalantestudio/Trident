#pragma once

#include <vulkan/vulkan.h>

#include "Engine/Renderer/TextureFormat.hpp"

namespace ProjectA {
	static VkFormat getVkFormat(TextureFormat textureFormat) {
		static constexpr VkFormat vkFormats[] = {
			VK_FORMAT_R8G8B8A8_SRGB,
		};

		return vkFormats[static_cast<sint32>(textureFormat)];
	}
}