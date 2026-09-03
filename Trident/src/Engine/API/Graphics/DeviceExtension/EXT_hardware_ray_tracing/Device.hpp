// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/DeviceExtension/DeviceExtension.hpp"
#include "Engine/API/Graphics/DeviceExtension/EXT_hardware_ray_tracing/AccelerationStructure.hpp"
#include "Engine/API/Graphics/DeviceExtension/EXT_hardware_ray_tracing/AccelerationStructureBuildGeometry.hpp"
#include "Engine/API/Graphics/DeviceExtension/EXT_hardware_ray_tracing/AccelerationStructureBuildSizes.hpp"
#include "Engine/API/Graphics/DeviceExtension/EXT_hardware_ray_tracing/Descriptor/AccelerationStructureBuildInstanceGeometryDescriptor.hpp"
#include "Engine/API/Graphics/DeviceExtension/EXT_hardware_ray_tracing/Descriptor/AccelerationStructureBuildTriangleGeometryDescriptor.hpp"
#include "Engine/API/Graphics/DeviceExtension/EXT_hardware_ray_tracing/Descriptor/AccelerationStructureDescriptor.hpp"

namespace Trident {
	template<typename Device>
	class DeviceExtension<EXT_hardware_ray_tracing, Device> {
		public:
			/// Creates and returns an acceleration structure.
			ref<AccelerationStructure> createAccelerationStructure(const AccelerationStructureDescriptor& descriptor) const;

			/// Creates and returns a build geometry for top-level acceleration structures.
			ref<AccelerationStructureBuildGeometry> createAccelerationStructureBuildGeometry(const AccelerationStructureBuildInstanceGeometryDescriptor& descriptor) const;

			/// Creates and returns a build geometry for bottom-level acceleration structures.
			ref<AccelerationStructureBuildGeometry> createAccelerationStructureBuildGeometry(const AccelerationStructureBuildTriangleGeometryDescriptor& descriptor) const;

			/// Calculates and returns the required sizes for acceleration structure related buffers:
			/// - Acceleration structure buffer
			/// - Scratch buffer (build size)
			/// - Scratch buffer (update size)
			AccelerationStructureBuildSizes getAccelerationStructureBuildSizes(ref<AccelerationStructureBuildGeometry> buildGeometry) const;
	};
}

#include "Engine/API/Graphics/DeviceExtension/EXT_hardware_ray_tracing/Device.ipp"