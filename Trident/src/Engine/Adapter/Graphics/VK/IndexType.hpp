// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace Trident::VK {
	inline VkIndexType translateIndexFormat(IndexFormat format) {
		static constexpr VkIndexType INDEX_TYPES[] = {
			VK_INDEX_TYPE_UINT16,
			VK_INDEX_TYPE_UINT32,
		};

		ADAPTER_ASSERT(format < IndexFormat::count, "Unsupported index format.");

		return INDEX_TYPES[static_cast<uint32>(format)];
	}
}