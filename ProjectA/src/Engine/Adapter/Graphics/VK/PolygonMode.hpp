#pragma once

#include "Engine/API/Rendering/PolygonMode.hpp"

namespace ProjectA {
	static VkPolygonMode getVkPolygonMode(PolygonMode polygonMode) {
		static constexpr VkPolygonMode vkPolygonModes[] = {
			VK_POLYGON_MODE_POINT,
			VK_POLYGON_MODE_LINE,
			VK_POLYGON_MODE_FILL,
		};

		return vkPolygonModes[static_cast<uint8>(polygonMode)];
	}
}