// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/DeviceExtension/EXT_hardware_ray_tracing/AccelerationStructureInstance.hpp"
#include "Engine/API/Graphics/Queue.hpp"

#include "AssetBuilder/API/Lightmapper/Lightmapper.hpp"

namespace Trident {
	void Lightmapper::createTopLevelAccelerationStructure(const BuildScene3D& scene) {
		const uint64 scratchAlignment = limits->minAccelerationStructureScratchOffsetAlignment;
		const uint32 instanceCount = static_cast<uint32>(scene.staticGeometry.instances.count());
		const BufferView<AccelerationStructureInstance> tlasInstanceView = tlasInstanceBuffer->createView<AccelerationStructureInstance>(0, tlasInstanceBuffer->getSize());

		const ref<AccelerationStructureBuildGeometry> buildInstanceGeometry = device->createAccelerationStructureBuildGeometry({
			.flags = AccelerationStructureBuildFlags::PREFER_FAST_TRACE,
			.geometryFlags = GeometryFlags::OPAQUE | GeometryFlags::NO_DUPLICATE_ANY_HIT_INVOCATION,
			.instanceView = tlasInstanceView,
			.instanceCount = instanceCount,
		});

		const AccelerationStructureBuildSizes topLevelAccelerationStructureBuildSizes = device->getAccelerationStructureBuildSizes(buildInstanceGeometry);

		tlasBuffer = device->createDynamicBuffer({
			.size = ceil(topLevelAccelerationStructureBuildSizes.accelerationStructureSize, 256ull),
			.usage = BufferUsage::SHADER_DEVICE_ADDRESS | BufferUsage::ACCELERATION_STRUCTURE_STORAGE,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL,
			.persistent = false,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "TLAS",
			#endif
		});

		tlasScratchBuffer = device->createDynamicBuffer({
			.size = ceil(topLevelAccelerationStructureBuildSizes.scratchBuildSize, scratchAlignment),
			.usage = BufferUsage::READ_ONLY_STORAGE | BufferUsage::SHADER_DEVICE_ADDRESS,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL,
			.persistent = false,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "TLAS scratch",
			#endif
		});

		const BufferView<uint8> view = tlasBuffer->createView<uint8>(0, ceil(topLevelAccelerationStructureBuildSizes.accelerationStructureSize, 256ull));

		tlasScratchView = tlasScratchBuffer->createView<uint8>(0, ceil(topLevelAccelerationStructureBuildSizes.scratchBuildSize, scratchAlignment));

		tlas = device->createAccelerationStructure({
			.view = view,
			.buildGeometry = buildInstanceGeometry,
		});
	}
}