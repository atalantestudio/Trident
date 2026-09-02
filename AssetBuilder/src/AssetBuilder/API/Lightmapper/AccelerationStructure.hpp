// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/Queue.hpp"

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

		const ref<AccelerationStructureBuildTriangleGeometry> buildTriangleGeometry = device->createAccelerationStructureBuildTriangleGeometry({
			.type = AccelerationStructureType::BOTTOM_LEVEL,
			.flags = {},
			.geometries = geometries,
		});

		const AccelerationStructureBuildSizes buildSizes = device->getAccelerationStructureBuildSizes(buildTriangleGeometry);
		const uint64 alignment = physicalDevice->getLimits().minAccelerationStructureScratchOffsetAlignment;

		accelerationStructureBuffer = device->createStaticBuffer({
			.size = ceil(buildSizes.accelerationStructureSize, 256ull),
			.usage = BufferUsage::SHADER_DEVICE_ADDRESS | BufferUsage::ACCELERATION_STRUCTURE_STORAGE,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL,
			.persistent = false,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Acceleration structure",
			#endif
		});

		const ref<Buffer> scratchBuffer = device->createStaticBuffer({
			.size = ceil(buildSizes.scratchBuildSize, alignment),
			.usage = BufferUsage::READ_ONLY_STORAGE | BufferUsage::SHADER_DEVICE_ADDRESS,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL,
			.persistent = false,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Scratch",
			#endif
		});

		ref<MemoryAllocator> memoryAllocator = device->getMemoryAllocator();
		uint64 allocationSize = 0;
		uint64 allocationAlignment = 0;

		const MemoryRequirements accelerationStructureBufferMemoryRequirements = memoryAllocator->getMemoryRequirements(accelerationStructureBuffer);

		allocationAlignment = max(allocationAlignment, accelerationStructureBufferMemoryRequirements.alignment);
		allocationSize += ceil(accelerationStructureBufferMemoryRequirements.size, allocationAlignment);

		const MemoryRequirements scratchBufferMemoryRequirements = memoryAllocator->getMemoryRequirements(scratchBuffer);

		allocationAlignment = max(allocationAlignment, scratchBufferMemoryRequirements.alignment);
		allocationSize += ceil(scratchBufferMemoryRequirements.size, allocationAlignment);

		const uint32 memoryTypeIndex = memoryAllocator->findMemoryTypeIndex(MemoryProperty::DEVICE_LOCAL, false);
		const uint32 allocationIndex = memoryAllocator->allocate(false, memoryTypeIndex, allocationSize, allocationAlignment, true);

		memoryAllocator->suballocateStatic(allocationIndex, accelerationStructureBuffer);
		memoryAllocator->suballocateStatic(allocationIndex, scratchBuffer);

		const BufferView<uint8> accelerationStructureView = accelerationStructureBuffer->createView<uint8>(0, accelerationStructureBuffer->getSize());
		const BufferView<uint8> scratchView = scratchBuffer->createView<uint8>(0, scratchBuffer->getSize());

		accelerationStructure = device->createAccelerationStructure({
			.view = accelerationStructureView,
			.buildTriangleGeometry = buildTriangleGeometry,
		});

		const ref<Queue> queue = device->getQueue(QueueFamilyCapability::GRAPHICS);
		const ref<ExtendedCommandEncoder<EXT_hardware_ray_tracing>> commandEncoder = queue->createCommandEncoder<EXT_hardware_ray_tracing>({
			.usage = CommandBufferUsage::ONE_TIME_SUBMIT,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Build acceleration structures",
			#endif
		});

		commandEncoder->begin();
		commandEncoder->buildAccelerationStructures(accelerationStructure, accelerationStructureView, scratchView);

		const ref<CommandBuffer> commandBuffer = commandEncoder->end();

		queue->submit(commandBuffer);
		queue->waitIdle();

		/*
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