// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "Engine/API/Graphics/Buffer.hpp"

namespace Trident {
	template<typename Device>
	ref<AccelerationStructure> DeviceExtension<EXT_hardware_ray_tracing, Device>::createAccelerationStructure(const AccelerationStructureDescriptor& descriptor) const {
		#if defined(DEBUG_ENABLE_GRAPHICS_ADAPTER_VALIDATION_LAYER)
			{
				Validator validator;

				if (descriptor.view.getBuffer()->getMemorySuballocation().allocationIndex == max<uint32>()) {
					VALIDATION_ERROR("descriptor.view doesn't belong to an allocated buffer.");
				}

				if (descriptor.view.getSize() % 256 > 0) {
					VALIDATION_ERROR("descriptor.view.size must be a multiple of 256.");
				}
			}
		#endif

		const Device* device = static_cast<const Device*>(this);

		return device->adapter->createAccelerationStructureImplementation(device, descriptor);
	}

	template<typename Device>
	ref<AccelerationStructureBuildGeometry> DeviceExtension<EXT_hardware_ray_tracing, Device>::createAccelerationStructureBuildGeometry(const AccelerationStructureBuildInstanceGeometryDescriptor& descriptor) const {
		#if defined(DEBUG_ENABLE_GRAPHICS_ADAPTER_VALIDATION_LAYER)
			{
				Validator validator;

				if (descriptor.instanceView.getBuffer()->getMemorySuballocation().allocationIndex == max<uint32>()) {
					VALIDATION_ERROR("descriptor.instanceView doesn't belong to an allocated buffer.");
				}
			}
		#endif

		const Device* device = static_cast<const Device*>(this);

		return device->adapter->createAccelerationStructureBuildInstanceGeometryImplementation(device, descriptor);
	}

	template<typename Device>
	ref<AccelerationStructureBuildGeometry> DeviceExtension<EXT_hardware_ray_tracing, Device>::createAccelerationStructureBuildGeometry(const AccelerationStructureBuildTriangleGeometryDescriptor& descriptor) const {
		#if defined(DEBUG_ENABLE_GRAPHICS_ADAPTER_VALIDATION_LAYER)
			{
				Validator validator;
				uint32 geometryIndex = 0;

				while (geometryIndex < descriptor.geometries.count()) {
					const AccelerationStructureTriangleGeometryDescriptor& geometry = descriptor.geometries[geometryIndex];
					const uint32 indexStride = geometry.indexView.getStride();

					if (indexStride == sizeof(uint16) && indexStride == sizeof(uint16)) {
						VALIDATION_ERROR("descriptor.geometries[[]].indexView must either have a 2 or 4 byte stride.", geometryIndex);
					}

					if (geometry.vertexView.getBuffer()->getMemorySuballocation().allocationIndex == max<uint32>()) {
						VALIDATION_ERROR("descriptor.geometries[[]].vertexView doesn't belong to an allocated buffer.", geometryIndex);
					}

					if (geometry.indexView.getBuffer()->getMemorySuballocation().allocationIndex == max<uint32>()) {
						VALIDATION_ERROR("descriptor.geometries[[]].indexView doesn't belong to an allocated buffer.", geometryIndex);
					}

					geometryIndex += 1;
				}
			}
		#endif

		const Device* device = static_cast<const Device*>(this);

		return device->adapter->createAccelerationStructureBuildTriangleGeometryImplementation(device, descriptor);
	}

	template<typename Device>
	AccelerationStructureBuildSizes DeviceExtension<EXT_hardware_ray_tracing, Device>::getAccelerationStructureBuildSizes(ref<AccelerationStructureBuildGeometry> buildGeometry) const {
		const Device* device = static_cast<const Device*>(this);

		return device->adapter->getAccelerationStructureBuildSizesImplementation(device, buildGeometry);
	}
}