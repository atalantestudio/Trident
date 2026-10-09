// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/VertexInputAttributeFormat.hpp"

namespace Trident::VK {
	inline VkFormat getFormat(VertexInputAttributeFormat format) {
		static constexpr VkFormat FORMATS[static_cast<uint32>(VertexInputAttributeFormat::count)] = {
			VK_FORMAT_R8_SINT,
			VK_FORMAT_R8_UINT,
			VK_FORMAT_A2B10G10R10_UNORM_PACK32,
			VK_FORMAT_R16_SINT,
			VK_FORMAT_R16_UINT,
			VK_FORMAT_R32_SINT,
			VK_FORMAT_R32_UINT,
			VK_FORMAT_R32_SFLOAT,
			VK_FORMAT_R16G16_UNORM,
			VK_FORMAT_R16G16_SFLOAT,
			VK_FORMAT_R32G32_SINT,
			VK_FORMAT_R32G32_UINT,
			VK_FORMAT_R32G32_SFLOAT,
			VK_FORMAT_R32G32B32_SINT,
			VK_FORMAT_R32G32B32_UINT,
			VK_FORMAT_R32G32B32_SFLOAT,
			VK_FORMAT_R32G32B32A32_SINT,
			VK_FORMAT_R32G32B32A32_UINT,
			VK_FORMAT_R32G32B32A32_SFLOAT,
		};

		ADAPTER_ASSERT(format < VertexInputAttributeFormat::count, "Invalid format.");

		return FORMATS[static_cast<uint32>(format)];
	}
}