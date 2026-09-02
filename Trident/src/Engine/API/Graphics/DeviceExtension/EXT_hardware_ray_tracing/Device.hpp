// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/DeviceExtension/DeviceExtension.hpp"
#include "Engine/API/Graphics/DeviceExtension/EXT_hardware_ray_tracing/AccelerationStructure.hpp"
#include "Engine/API/Graphics/DeviceExtension/EXT_hardware_ray_tracing/AccelerationStructureBuildTriangleGeometry.hpp"
#include "Engine/API/Graphics/DeviceExtension/EXT_hardware_ray_tracing/AccelerationStructureBuildSizes.hpp"
#include "Engine/API/Graphics/DeviceExtension/EXT_hardware_ray_tracing/Descriptor/AccelerationStructureBuildTriangleGeometryDescriptor.hpp"
#include "Engine/API/Graphics/DeviceExtension/EXT_hardware_ray_tracing/Descriptor/AccelerationStructureDescriptor.hpp"

namespace Trident {
	template<typename Device>
	class DeviceExtension<EXT_hardware_ray_tracing, Device> {
		public:
			ref<AccelerationStructure> createAccelerationStructure(const AccelerationStructureDescriptor& descriptor) const;

			ref<AccelerationStructureBuildTriangleGeometry> createAccelerationStructureBuildTriangleGeometry(const AccelerationStructureBuildTriangleGeometryDescriptor& descriptor) const;

			AccelerationStructureBuildSizes getAccelerationStructureBuildSizes(ref<AccelerationStructureBuildTriangleGeometry> buildTriangleGeometry) const;
	};
}

#include "Engine/API/Graphics/DeviceExtension/EXT_hardware_ray_tracing/Device.ipp"