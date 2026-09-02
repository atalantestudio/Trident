// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/DeviceExtension/EXT_hardware_ray_tracing/AccelerationStructureBuildFlags.hpp"
#include "Engine/API/Graphics/DeviceExtension/EXT_hardware_ray_tracing/AccelerationStructureType.hpp"
#include "Engine/API/Graphics/DeviceExtension/EXT_hardware_ray_tracing/Descriptor/AccelerationStructureTriangleGeometryDescriptor.hpp"

namespace Trident {
	struct AccelerationStructureBuildTriangleGeometryDescriptor {
		AccelerationStructureType type;
		AccelerationStructureBuildFlags flags;
		view<AccelerationStructureTriangleGeometryDescriptor> geometries;
	};
}