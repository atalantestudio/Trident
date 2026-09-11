// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace Trident::VK {
	inline VkIndexType translateIndexFormat(IndexFormat format) {
		switch (format) {
			case R16_UINT: {
				return VK_INDEX_TYPE_UINT16;
			}
			case R32_UINT: {
				return VK_INDEX_TYPE_UINT32;
			}
			default: {
				ADAPTER_ASSERT(false, "Unsupported index format.");

				return VK_INDEX_TYPE_MAX_ENUM;
			}
		}
	}
}