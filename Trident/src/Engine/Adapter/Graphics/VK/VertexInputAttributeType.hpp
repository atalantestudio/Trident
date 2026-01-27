// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/VertexInputAttributeType.hpp"

namespace Trident::VK {
	inline VkFormat getFormat(VertexInputAttributeType vertexInputAttributeType) {
		static constexpr VkFormat VERTEX_INPUT_ATTRIBUTE_TYPES[] = {
			VK_FORMAT_R8_SINT,
			VK_FORMAT_R8_UINT,
			VK_FORMAT_R16_SINT,
			VK_FORMAT_R16_UINT,
			VK_FORMAT_R32_SINT,
			VK_FORMAT_R32_UINT,
			VK_FORMAT_R32_SFLOAT,
			VK_FORMAT_R32G32_SFLOAT,
			VK_FORMAT_R32G32B32_SFLOAT,
			VK_FORMAT_R32G32B32A32_SFLOAT,
			VK_FORMAT_R64_SINT,
			VK_FORMAT_R64_UINT,
			VK_FORMAT_R64_SFLOAT,
			VK_FORMAT_R64G64_SFLOAT,
			VK_FORMAT_R64G64B64_SFLOAT,
			VK_FORMAT_R64G64B64A64_SFLOAT,
		};

		return VERTEX_INPUT_ATTRIBUTE_TYPES[static_cast<uint32>(vertexInputAttributeType)];
	}
}