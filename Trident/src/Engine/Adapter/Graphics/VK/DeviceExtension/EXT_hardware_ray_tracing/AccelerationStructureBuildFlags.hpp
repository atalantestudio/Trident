// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/DeviceExtension/EXT_hardware_ray_tracing/AccelerationStructureBuildFlags.hpp"

namespace Trident::VK {
	inline VkBuildAccelerationStructureFlagsKHR translateAccelerationStructureBuildFlags(AccelerationStructureBuildFlags flags) {
		VkBuildAccelerationStructureFlagsKHR vkFlags = 0;

		if (flags & AccelerationStructureBuildFlags::ALLOW_UPDATE) {
			vkFlags |= VK_BUILD_ACCELERATION_STRUCTURE_ALLOW_UPDATE_BIT_KHR;
		}

		if (flags & AccelerationStructureBuildFlags::ALLOW_COMPACTION) {
			vkFlags |= VK_BUILD_ACCELERATION_STRUCTURE_ALLOW_COMPACTION_BIT_KHR;
		}

		if (flags & AccelerationStructureBuildFlags::PREFER_FAST_TRACE) {
			vkFlags |= VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
		}

		if (flags & AccelerationStructureBuildFlags::PREFER_FAST_BUILD) {
			vkFlags |= VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_BUILD_BIT_KHR;
		}

		if (flags & AccelerationStructureBuildFlags::MINIMIZE_MEMORY) {
			vkFlags |= VK_BUILD_ACCELERATION_STRUCTURE_LOW_MEMORY_BIT_KHR;
		}

		return vkFlags;
	}
}