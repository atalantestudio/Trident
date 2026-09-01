// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/DeviceExtension/DeviceExtension.hpp"
#include "Engine/API/Graphics/DeviceExtension/EXT_hardware_ray_tracing/AccelerationStructureBuildGeometryDescriptor.hpp"
#include "Engine/API/Graphics/DeviceExtension/EXT_hardware_ray_tracing/AccelerationStructureBuildSizes.hpp"

namespace Trident {
	template<typename Device>
	class DeviceExtension<EXT_hardware_ray_tracing, Device> {
		public:
			AccelerationStructureBuildSizes getAccelerationStructureBuildSizes(const AccelerationStructureBuildGeometryDescriptor& descriptor) const;
	};
}

#include "Engine/API/Graphics/DeviceExtension/EXT_hardware_ray_tracing/Device.ipp"