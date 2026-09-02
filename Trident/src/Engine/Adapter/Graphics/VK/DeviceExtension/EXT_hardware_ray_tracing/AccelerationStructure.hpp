// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/DeviceExtension/EXT_hardware_ray_tracing/AccelerationStructure.hpp"

namespace Trident::VK {
	class AccelerationStructure final : public Trident::AccelerationStructure {
		public:
			VkAccelerationStructureKHR accelerationStructure;
			VkAccelerationStructureBuildGeometryInfoKHR buildGeometryInfo;
			const VkAccelerationStructureBuildRangeInfoKHR* buildRangeInfos;
	};
}