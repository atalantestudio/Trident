// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/DeviceExtension/EXT_hardware_ray_tracing/AccelerationStructureBuildTriangleGeometry.hpp"

namespace Trident::VK {
	class AccelerationStructureBuildTriangleGeometry final : public Trident::AccelerationStructureBuildTriangleGeometry {
		public:
			sequence<VkAccelerationStructureGeometryKHR> geometries;
			sequence<VkAccelerationStructureBuildRangeInfoKHR> buildRangeInfos;
			sequence<uint32> maxPrimitiveCounts;
			VkAccelerationStructureBuildGeometryInfoKHR buildGeometryInfo;
	};
}