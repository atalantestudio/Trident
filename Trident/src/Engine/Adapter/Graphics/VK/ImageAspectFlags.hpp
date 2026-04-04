// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/VK/ImageFormat.hpp"

namespace Trident::VK {
	inline VkImageAspectFlags getImageAspectFlags(VkFormat format) {
		VkImageAspectFlags aspectFlags = VK_IMAGE_ASPECT_NONE;

		if (!isDepthFormat(format) && !isStencilFormat(format)) {
			aspectFlags |= VK_IMAGE_ASPECT_COLOR_BIT;
		}

		if (isDepthFormat(format)) {
			aspectFlags |= VK_IMAGE_ASPECT_DEPTH_BIT;
		}

		if (isStencilFormat(format)) {
			aspectFlags |= VK_IMAGE_ASPECT_STENCIL_BIT;
		}

		return aspectFlags;
	}
}