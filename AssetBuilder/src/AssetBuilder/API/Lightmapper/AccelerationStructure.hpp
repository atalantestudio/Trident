// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Lightmapper/Lightmapper.hpp"

namespace Trident {
	void Lightmapper::buildAccelerationStructures(const BuildScene3D& scene) {
		sequence<AccelerationStructureTriangleGeometryDescriptor> geometries(scene.staticGeometry.geometryCount);
		uint32 geometryIndex = 0;

		while (geometryIndex < scene.staticGeometry.geometryCount) {
			const BuildGeometry& geometry = scene.geometries[geometryIndex];

			geometries[geometryIndex] = {
				.flags = GeometryFlags::OPAQUE | GeometryFlags::NO_DUPLICATE_ANY_HIT_INVOCATION,
				.vertexView = context.vertexBuffer->createView<BuildVertex3D>(geometry.vertexRange.offset * sizeof(BuildVertex3D), geometry.vertexRange.count * sizeof(BuildVertex3D)),
				.vertexFormat = VertexInputAttributeFormat::R32G32B32_FLOAT,
				.indexView = context.indexBuffer->createView<uint32>(geometry.indexRange.offset * sizeof(uint32), geometry.indexRange.count * sizeof(uint32)),
			};

			geometryIndex += 1;
		}

		const AccelerationStructureBuildGeometryDescriptor build = {
			.type = AccelerationStructureType::BOTTOM_LEVEL,
			.flags = {},
			.geometries = geometries,
		};

		const AccelerationStructureBuildSizes buildSizes = device->getAccelerationStructureBuildSizes(build);
		const uint64 alignment = physicalDevice->getLimits().minAccelerationStructureScratchOffsetAlignment;

		const ref<Buffer> scratchBuffer = device->createDynamicBuffer({
			.size = ceil(buildSizes.scratchBuildSize, alignment),
			.usage = BufferUsage::ACCELERATION_STRUCTURE_STORAGE,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL,
			.persistent = false,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Scratch",
			#endif
		});

		/*

		- Create aligned scratch buffer with STORAGE, SHADER_DEVICE_ADDRESS and ACCELERATION_STRUCTURE_STORAGE usage bits
		- (Vulkan) Create acceleration structure
		- Build acceleration structure in command buffer
		- Destroy scratch buffer

		For each geometry
			Create VkAccelerationStructureGeometryKHR
			Create VkAccelerationStructureBuildRangeInfoKHR
			Create corresponding BLAS

		For each instance
			Create VkAccelerationStructureInstanceKHR

		Create TLAS instance buffer
		Copy instances to TLAS instance buffer

		Create VkAccelerationStructureGeometryInstancesDataKHR
		Create TLAS



		On release:
			For each BLAS
				Destroy BLAS

			Destroy TLAS
		*/

		ENGINE_ERROR_UNIMPLEMENTED();
	}
}