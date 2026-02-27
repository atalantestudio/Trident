// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/GraphicsAdapter.hpp"
#include "Engine/API/Rendering/ImageFormat.hpp"

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
		switch (format) {
			case VK_FORMAT_R8_UNORM: {
				return ImageFormat::R8_UNORM;
			}
			case VK_FORMAT_R8G8B8_UNORM: {
				return ImageFormat::R8G8B8_UNORM;
			}
			case VK_FORMAT_B8G8R8A8_UNORM: {
				return ImageFormat::B8G8R8A8_UNORM;
			}
			case VK_FORMAT_R8G8B8A8_SRGB: {
				return ImageFormat::R8G8B8A8_SRGB;
			}
			case VK_FORMAT_R8G8B8A8_UNORM: {
				return ImageFormat::R8G8B8A8_UNORM;
			}
			case VK_FORMAT_R32_UINT: {
				return ImageFormat::R32_UINT;
			}
			case VK_FORMAT_R32G32_UINT: {
				return ImageFormat::R32G32_UINT;
			}
			case VK_FORMAT_D32_SFLOAT: {
				return ImageFormat::D32_SFLOAT;
			}
			case VK_FORMAT_D32_SFLOAT_S8_UINT: {
				return ImageFormat::D32_SFLOAT_S8_UINT;
			}
			case VK_FORMAT_D24_UNORM_S8_UINT: {
				return ImageFormat::D24_UNORM_S8_UINT;
			}
			default: {
				ADAPTER_ASSERT(false, "Invalid format.");

				return static_cast<ImageFormat>(0);
			}
		}
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