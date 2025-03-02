#pragma once

#include <vulkan/vulkan.h>

#include "Engine/Renderer/Buffer/VertexInputAttributeType.hpp"

namespace ProjectA {
	static inline VkFormat getVkFormat(const VertexInputAttributeType vertexInputAttributeType) {
		static constexpr VkFormat vkFormats[] = {
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

		return vkFormats[static_cast<uint32>(vertexInputAttributeType)];
	}
}