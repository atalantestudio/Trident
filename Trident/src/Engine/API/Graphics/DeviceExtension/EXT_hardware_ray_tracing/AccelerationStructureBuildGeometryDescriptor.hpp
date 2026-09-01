// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/DeviceExtension/EXT_hardware_ray_tracing/AccelerationStructureBuildFlags.hpp"
#include "Engine/API/Graphics/DeviceExtension/EXT_hardware_ray_tracing/AccelerationStructureTriangleGeometryDescriptor.hpp"
#include "Engine/API/Graphics/DeviceExtension/EXT_hardware_ray_tracing/AccelerationStructureType.hpp"

namespace Trident {
	struct AccelerationStructureBuildGeometryDescriptor {
		AccelerationStructureType type;
		AccelerationStructureBuildFlags flags;
		view<AccelerationStructureTriangleGeometryDescriptor> geometries;
	};
}