// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/PrimitiveTopology.hpp"

namespace Trident::VK {
	inline VkPrimitiveTopology getPrimitiveTopology(PrimitiveTopology primitiveTopology) {
		static constexpr VkPrimitiveTopology PRIMITIVE_TOPOLOGIES[] = {
			VK_PRIMITIVE_TOPOLOGY_POINT_LIST,
			VK_PRIMITIVE_TOPOLOGY_LINE_LIST,
			VK_PRIMITIVE_TOPOLOGY_LINE_STRIP,
			VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
			VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP,
		};

		ADAPTER_ASSERT(primitiveTopology < PrimitiveTopology::count, "Invalid primitive topology.");

		return PRIMITIVE_TOPOLOGIES[static_cast<uint32>(primitiveTopology)];
	}
}