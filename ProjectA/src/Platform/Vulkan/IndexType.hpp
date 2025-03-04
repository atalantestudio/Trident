#pragma once

#include <vulkan/vulkan.h>

#include "Engine/Renderer/IndexType.hpp"

namespace ProjectA {
	static inline VkIndexType getVkIndexType(const IndexType indexType) {
		static constexpr VkIndexType vkIndexTypes[] = {
			VK_INDEX_TYPE_UINT16,
			VK_INDEX_TYPE_UINT32,
		};

		return vkIndexTypes[static_cast<uint32>(indexType)];
	}
}