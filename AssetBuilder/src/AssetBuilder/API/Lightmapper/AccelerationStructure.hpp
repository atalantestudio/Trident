// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/DeviceExtension/EXT_hardware_ray_tracing/AccelerationStructureInstance.hpp"
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
			.flags = AccelerationStructureBuildFlags::PREFER_FAST_TRACE,
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

		const ref<Buffer> instanceBuffer = device->createStaticBuffer({
			.size = ceil(scene.staticGeometry.instances.count() * sizeof(AccelerationStructureInstance), 256ull),
			.usage = BufferUsage::TRANSFER_DST | BufferUsage::SHADER_DEVICE_ADDRESS | BufferUsage::READ_ONLY_ACCELERATION_STRUCTURE_INPUT,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL,
			.persistent = false,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Instance",
			#endif
		});

		const ref<Buffer> instanceStagingBuffer = device->createDynamicBuffer({
			.size = scene.staticGeometry.instances.count() * sizeof(AccelerationStructureInstance),
			.usage = BufferUsage::TRANSFER_SRC,
			.memoryProperties = MemoryProperty::HOST_VISIBLE | MemoryProperty::HOST_COHERENT,
			.persistent = false,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Instance staging",
			#endif
		});

		ref<MemoryAllocator> memoryAllocator = device->getMemoryAllocator();
		MemoryRequirements bufferMemoryRequirements;
		uint64 allocationSize = 0;
		uint64 allocationAlignment = 0;

		bufferMemoryRequirements = memoryAllocator->getMemoryRequirements(accelerationStructureBuffer);
		allocationAlignment = max(allocationAlignment, bufferMemoryRequirements.alignment);
		allocationSize += ceil(bufferMemoryRequirements.size, allocationAlignment);

		bufferMemoryRequirements = memoryAllocator->getMemoryRequirements(scratchBuffer);
		allocationAlignment = max(allocationAlignment, bufferMemoryRequirements.alignment);
		allocationSize += ceil(bufferMemoryRequirements.size, allocationAlignment);

		bufferMemoryRequirements = memoryAllocator->getMemoryRequirements(instanceBuffer);
		allocationAlignment = max(allocationAlignment, bufferMemoryRequirements.alignment);
		allocationSize += ceil(bufferMemoryRequirements.size, allocationAlignment);

		const uint32 memoryTypeIndex = memoryAllocator->findMemoryTypeIndex(MemoryProperty::DEVICE_LOCAL, false);
		const uint32 allocationIndex = memoryAllocator->allocate(false, memoryTypeIndex, allocationSize, allocationAlignment, true);

		memoryAllocator->suballocateStatic(allocationIndex, accelerationStructureBuffer);
		memoryAllocator->suballocateStatic(allocationIndex, scratchBuffer);
		memoryAllocator->suballocateStatic(allocationIndex, instanceBuffer);

		const BufferView<uint8> accelerationStructureView = accelerationStructureBuffer->createView<uint8>(0, accelerationStructureBuffer->getSize());
		const BufferView<uint8> scratchView = scratchBuffer->createView<uint8>(0, scratchBuffer->getSize());
		const BufferView<AccelerationStructureInstance> instanceView = instanceBuffer->createView<AccelerationStructureInstance>(0, instanceBuffer->getSize());
		const BufferView<AccelerationStructureInstance> instanceStagingView = instanceStagingBuffer->createView<AccelerationStructureInstance>(0, instanceStagingBuffer->getSize());

		accelerationStructure = device->createAccelerationStructure({
			.view = accelerationStructureView,
			.buildTriangleGeometry = buildTriangleGeometry,
		});

		AccelerationStructureInstance* instanceMappedRange = instanceStagingView.map();

		uint32 materialGeometryIndex = 0;

		while (materialGeometryIndex < scene.materialGeometries.count()) {
			const MaterialGeometry& materialGeometry = scene.materialGeometries[materialGeometryIndex];

			if (materialGeometry.geometryIndex >= scene.staticGeometry.geometryCount) {
				materialGeometryIndex += 1;

				continue;
			}

			uint32 instanceIndex = 0;

			while (instanceIndex < materialGeometry.instanceRange.count) {
				const BuildTransform3D& transform = scene.staticGeometry.instances[materialGeometry.instanceRange.offset + instanceIndex];
				const quat<float32> orientation = normalized(
					quat<float32>::axisAngle(UP, transform.rotation[1]) *
					quat<float32>::axisAngle(RIGHT, transform.rotation[0]) *
					quat<float32>::axisAngle(FORWARD, transform.rotation[2])
				);
				const mat4x4<float32> T = transpose(
					mat4x4<float32>(orientation) *
					mat4x4<float32>::translation(transform.translation) * 
					mat4x4<float32>::scaling({transform.scale, transform.scale, transform.scale})
				);

				instanceMappedRange[instanceIndex] = {
					{
						{T[0][0], T[0][1], T[0][2], T[0][3]},
						{T[1][0], T[1][1], T[1][2], T[1][3]},
						{T[2][0], T[2][1], T[2][2], T[2][3]},
					},
				};

				AccelerationStructureInstance& instance = instanceMappedRange[instanceIndex];
				instance.index = instanceIndex;
				instance.mask = max<uint8>();
				instance.shaderBindingTableRecordOffset = 0;
				instance.accelerationStructureAddress = accelerationStructure->getDeviceAddress();

				instanceIndex += 1;
			}

			materialGeometryIndex += 1;
		}

		instanceStagingBuffer->unmap();

		const ref<Queue> queue = device->getQueue(QueueFamilyCapability::GRAPHICS);
		const ref<ExtendedCommandEncoder<EXT_hardware_ray_tracing>> commandEncoder = queue->createCommandEncoder<EXT_hardware_ray_tracing>({
			.usage = CommandBufferUsage::ONE_TIME_SUBMIT,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Build acceleration structures",
			#endif
		});

		commandEncoder->begin();
		commandEncoder->buildAccelerationStructures(accelerationStructure, accelerationStructureView, scratchView);

		const BufferCopyRegion instanceCopyRegion = {
			.sourceOffset = instanceStagingView.getOffset(),
			.destinationOffset = instanceView.getOffset(),
			.size = instanceStagingView.getSize(),
		};

		commandEncoder->copyBufferToBuffer(instanceStagingBuffer, instanceBuffer, instanceCopyRegion);

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