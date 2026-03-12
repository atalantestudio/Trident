// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/ImageLayout.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace Trident::VK {
	inline VkImageLayout getImageLayout(ImageLayout imageLayout) {
		static constexpr VkImageLayout IMAGE_LAYOUTS[] = {
			VK_IMAGE_LAYOUT_UNDEFINED,
			VK_IMAGE_LAYOUT_GENERAL,
			VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
			VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
			VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,
			VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
			VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
		};

		ADAPTER_ASSERT(imageLayout < ImageLayout::count, "Invalid image layout.");

		return IMAGE_LAYOUTS[static_cast<uint32>(imageLayout)];
	}

	inline VkAccessFlags getAccessFlags(ImageLayout imageLayout) {
		static constexpr VkAccessFlags ACCESS_FLAGS[] = {
			VK_ACCESS_NONE,
			VK_ACCESS_NONE,
			VK_ACCESS_COLOR_ATTACHMENT_READ_BIT,
			VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT,
			VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT,
			VK_ACCESS_SHADER_READ_BIT,
			VK_ACCESS_TRANSFER_READ_BIT,
			VK_ACCESS_TRANSFER_WRITE_BIT,
			VK_ACCESS_NONE,
		};

		ADAPTER_ASSERT(imageLayout < ImageLayout::count, "Invalid image layout.");
		ADAPTER_ASSERT(imageLayout != ImageLayout::PRESENT_SRC, "PRESENT_SRC cannot be used with access flags.");

		return ACCESS_FLAGS[static_cast<uint32>(imageLayout)];
	}
}