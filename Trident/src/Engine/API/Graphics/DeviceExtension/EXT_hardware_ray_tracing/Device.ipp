// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	template<typename Device>
	AccelerationStructureBuildSizes DeviceExtension<EXT_hardware_ray_tracing, Device>::getAccelerationStructureBuildSizes(const AccelerationStructureBuildGeometryDescriptor& descriptor) const {
		#if defined(DEBUG_ENABLE_GRAPHICS_ADAPTER_VALIDATION_LAYER)
			{
				Validator validator;
				uint32 geometryIndex = 0;

				while (geometryIndex < descriptor.geometries.count()) {
					const AccelerationStructureTriangleGeometryDescriptor& geometry = descriptor.geometries[geometryIndex];

					if (geometry.vertex.count == 0) {
						VALIDATION_ERROR("descriptor.geometries[[]].count must not be 0.", geometryIndex);
					}

					geometryIndex += 1;
				}
			}
		#endif

		const Device* device = static_cast<const Device*>(this);

		return device->adapter->getAccelerationStructureBuildSizesImplementation(device, descriptor);
	}
}