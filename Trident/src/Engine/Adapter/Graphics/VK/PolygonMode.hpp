// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/PolygonMode.hpp"

namespace Trident::VK {
	inline VkPolygonMode getPolygonMode(PolygonMode polygonMode) {
		static constexpr VkPolygonMode POLYGON_MODES[] = {
			VK_POLYGON_MODE_POINT,
			VK_POLYGON_MODE_LINE,
			VK_POLYGON_MODE_FILL,
		};

		ADAPTER_ASSERT(polygonMode < PolygonMode::count, "Invalid polygon mode.");

		return POLYGON_MODES[static_cast<uint32>(polygonMode)];
	}
}