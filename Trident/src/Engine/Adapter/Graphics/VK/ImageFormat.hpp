// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/ImageFormat.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace Trident::VK {
	inline VkFormat getFormat(ImageFormat imageFormat) {
		static constexpr VkFormat FORMATS[] = {
			VK_FORMAT_R8_UNORM,
			VK_FORMAT_R8G8B8_UNORM,
			VK_FORMAT_B8G8R8A8_UNORM,
			VK_FORMAT_R8G8B8A8_SRGB,
			VK_FORMAT_R8G8B8A8_UNORM,
			VK_FORMAT_R32_UINT,
			VK_FORMAT_R32G32_UINT,
			VK_FORMAT_D32_SFLOAT,
			VK_FORMAT_D32_SFLOAT_S8_UINT,
			VK_FORMAT_D24_UNORM_S8_UINT,
		};

		ADAPTER_ASSERT(imageFormat < ImageFormat::count, "Invalid image format.");

		return FORMATS[static_cast<uint32>(imageFormat)];
	}

	inline ImageFormat fromFormat(VkFormat format) {
		static constexpr ImageFormat IMAGE_FORMATS[] = {
			ImageFormat::R8_UNORM,
			ImageFormat::R8G8B8_UNORM,
			ImageFormat::B8G8R8A8_UNORM,
			ImageFormat::R8G8B8A8_SRGB,
			ImageFormat::R8G8B8A8_UNORM,
			ImageFormat::R32_UINT,
			ImageFormat::R32G32_UINT,
			ImageFormat::D32_SFLOAT,
			ImageFormat::D32_SFLOAT_S8_UINT,
			ImageFormat::D24_UNORM_S8_UINT,
		};

		ADAPTER_ASSERT(imageFormat < ImageFormat::count, "Invalid image format.");

		return IMAGE_FORMATS[static_cast<uint32>(format)];
	}

	inline bool isDepthFormat(VkFormat format) {
		return (
			format == VK_FORMAT_D32_SFLOAT ||
			format == VK_FORMAT_D32_SFLOAT_S8_UINT ||
			format == VK_FORMAT_D24_UNORM_S8_UINT
		);
	}

	inline bool isStencilFormat(VkFormat format) {
		return (
			format == VK_FORMAT_S8_UINT ||
			format == VK_FORMAT_D16_UNORM_S8_UINT ||
			format == VK_FORMAT_D24_UNORM_S8_UINT ||
			format == VK_FORMAT_D32_SFLOAT_S8_UINT
		);
	}
}