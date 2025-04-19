#pragma once

#include "Engine/Renderer/IndexType.hpp"

namespace ProjectA {
	static VkIndexType getVkIndexType(IndexType indexType) {
		static constexpr VkIndexType vkIndexTypes[] = {
			VK_INDEX_TYPE_UINT16,
			VK_INDEX_TYPE_UINT32,
		};

		return vkIndexTypes[static_cast<uint8>(indexType)];
	}
}