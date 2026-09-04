// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/Device.hpp"
#include "Engine/API/Graphics/DeviceExtension/EXT_hardware_ray_tracing/EXT_hardware_ray_tracing.hpp"

namespace Trident::DX12 {
	ref<Trident::AccelerationStructure> createAccelerationStructure(const Trident::Device* device, const AccelerationStructureDescriptor& descriptor);

	ref<Trident::AccelerationStructureBuildGeometry> createAccelerationStructureBuildInstanceGeometry(const Trident::Device* device, const AccelerationStructureBuildInstanceGeometryDescriptor& descriptor);

	ref<Trident::AccelerationStructureBuildGeometry> createAccelerationStructureBuildTriangleGeometry(const Trident::Device* device, const AccelerationStructureBuildTriangleGeometryDescriptor& descriptor);

	ref<Trident::RayTracingPipeline> createRayTracingPipeline(const Trident::Device* device, const RayTracingPipelineDescriptor& descriptor);

	AccelerationStructureBuildSizes getAccelerationStructureBuildSizes(const Trident::Device* device, ref<Trident::AccelerationStructureBuildGeometry> buildGeometry);

	uint64 getBufferDeviceAddress(const Trident::Device* device, ref<Trident::Buffer> buffer);
}