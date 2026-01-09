// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/PolygonMode.hpp"

namespace ProjectA::VK {
	inline VkPolygonMode getPolygonMode(PolygonMode polygonMode) {
		static constexpr VkPolygonMode vkPolygonModes[] = {
			VK_POLYGON_MODE_POINT,
			VK_POLYGON_MODE_LINE,
			VK_POLYGON_MODE_FILL,
		};

		return vkPolygonModes[static_cast<uint8>(polygonMode)];
	}
}