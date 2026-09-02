// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/Device.hpp"
#include "Engine/API/Graphics/DeviceExtension/EXT_hardware_ray_tracing/EXT_hardware_ray_tracing.hpp"

namespace Trident::VK {
	ref<Trident::AccelerationStructure> createAccelerationStructure(const Trident::Device* device, const AccelerationStructureDescriptor& descriptor);

	ref<Trident::AccelerationStructureBuildTriangleGeometry> createAccelerationStructureBuildTriangleGeometry(const Trident::Device* device, const AccelerationStructureBuildTriangleGeometryDescriptor& descriptor);

	AccelerationStructureBuildSizes getAccelerationStructureBuildSizes(const Trident::Device* device, ref<Trident::AccelerationStructureBuildTriangleGeometry> buildTriangleGeometry);
}