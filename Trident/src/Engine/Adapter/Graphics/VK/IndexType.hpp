// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace Trident::VK {
	inline VkIndexType getIndexType(uint8 indexTypeSize) {
		switch (indexTypeSize) {
			case sizeof(uint16):
				return VK_INDEX_TYPE_UINT16;
			case sizeof(uint32):
				return VK_INDEX_TYPE_UINT32;
			default: {
				ADAPTER_ASSERT(false, "Unsupported index type size.");

				return static_cast<VkIndexType>(0);
			}
		}
	}
}