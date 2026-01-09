// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/PrimitiveTopology.hpp"

namespace ProjectA::VK {
	inline VkPrimitiveTopology getPrimitiveTopology(PrimitiveTopology primitiveTopology) {
		static constexpr VkPrimitiveTopology vkPrimitiveTopologies[] = {
			VK_PRIMITIVE_TOPOLOGY_POINT_LIST,
			VK_PRIMITIVE_TOPOLOGY_LINE_LIST,
			VK_PRIMITIVE_TOPOLOGY_LINE_STRIP,
			VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
			VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP,

			// Only if the VK_KHR_portability_subset extension is NOT enabled.
			// VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN,

			// Only if the geometryShader feature is enabled.
			// VK_PRIMITIVE_TOPOLOGY_LINE_LIST_WITH_ADJACENCY,
			// VK_PRIMITIVE_TOPOLOGY_LINE_STRIP_WITH_ADJACENCY,
			// VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY,
			// VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY,

			// Only if the tessellationShader feature is enabled.
			// VK_PRIMITIVE_TOPOLOGY_PATCH_LIST,
		};

		return vkPrimitiveTopologies[static_cast<uint8>(primitiveTopology)];
	}
}