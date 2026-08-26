// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/Device.hpp"
#include "Engine/API/Graphics/DeviceExtension/DeviceExtension.hpp"
#include "Engine/API/Graphics/DeviceExtension/EXT_hardware_ray_tracing/BLASTriangleGeometryDescriptor.hpp"

namespace Trident {
	template<typename Device>
	class DeviceExtension<EXT_hardware_ray_tracing, Device> {
		public:
			void testCreateBLASTriangleGeometry(const BLASTriangleGeometryDescriptor& descriptor) {
				Device* device = static_cast<Device*>(this);

				device->adapter->testCreateBLASTriangleGeometryImplementation(device, descriptor);
			}
	};
}