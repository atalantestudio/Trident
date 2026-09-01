// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/DeviceExtension/EXT_hardware_ray_tracing/AccelerationStructureType.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace Trident::VK {
	inline VkAccelerationStructureTypeKHR translateAccelerationStructureType(AccelerationStructureType type) {
		static constexpr VkAccelerationStructureTypeKHR ACCELERATION_STRUCTURE_TYPES[] = {
			VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR,
    		VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR,
		};

		ADAPTER_ASSERT(type < AccelerationStructureType::count, "Invalid acceleration structure type.");

		return ACCELERATION_STRUCTURE_TYPES[static_cast<uint32>(type)];
	}
}