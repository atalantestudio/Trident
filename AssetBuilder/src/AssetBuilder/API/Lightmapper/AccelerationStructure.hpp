// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/DeviceExtension/EXT_hardware_ray_tracing/AccelerationStructureInstance.hpp"
#include "Engine/API/Graphics/Queue.hpp"

#include "AssetBuilder/API/Lightmapper/Lightmapper.hpp"

namespace Trident {
	void Lightmapper::buildAccelerationStructures(const BuildScene3D& scene) {
		const uint64 scratchAlignment = physicalDevice->getLimits().minAccelerationStructureScratchOffsetAlignment;
		const uint64 storageBufferAlignment = physicalDevice->getLimits().minStorageBufferOffsetAlignment;

		const uint32 instanceCount = static_cast<uint32>(scene.staticGeometry.instances.count());

		vertexBuffer = device->createDynamicBuffer({
			.size = ceil(scene.staticGeometry.vertexCount * sizeof(BuildVertex3D), storageBufferAlignment),
			.usage = BufferUsage::TRANSFER_DST | BufferUsage::READ_ONLY_STORAGE | BufferUsage::SHADER_DEVICE_ADDRESS | BufferUsage::READ_ONLY_ACCELERATION_STRUCTURE_INPUT,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL,
			.persistent = false,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Vertex",
			#endif
		});

		indexBuffer = device->createDynamicBuffer({
			.size = ceil(scene.staticGeometry.indexCount * sizeof(uint32), storageBufferAlignment),
			.usage = BufferUsage::TRANSFER_DST | BufferUsage::READ_ONLY_STORAGE | BufferUsage::SHADER_DEVICE_ADDRESS | BufferUsage::READ_ONLY_ACCELERATION_STRUCTURE_INPUT,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL,
			.persistent = false,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Index",
			#endif
		});

		const ref<Buffer> instanceBuffer = device->createDynamicBuffer({
			.size = ceil(instanceCount * sizeof(AccelerationStructureInstance), 256ull),
			.usage = BufferUsage::TRANSFER_DST | BufferUsage::SHADER_DEVICE_ADDRESS | BufferUsage::READ_ONLY_ACCELERATION_STRUCTURE_INPUT,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL,
			.persistent = false,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Instance",
			#endif
		});

		const ref<Buffer> instanceStagingBuffer = device->createDynamicBuffer({
			.size = instanceCount * sizeof(AccelerationStructureInstance),
			.usage = BufferUsage::TRANSFER_SRC,
			.memoryProperties = MemoryProperty::HOST_VISIBLE | MemoryProperty::HOST_COHERENT,
			.persistent = false,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Instance staging",
			#endif
		});

		const BufferView<AccelerationStructureInstance> instanceView = instanceBuffer->createView<AccelerationStructureInstance>(0, instanceBuffer->getSize());

		sequence<AccelerationStructureTriangleGeometryDescriptor> geometries(scene.staticGeometry.geometryCount);
		uint32 geometryIndex = 0;

		while (geometryIndex < scene.staticGeometry.geometryCount) {
			const BuildGeometry& geometry = scene.geometries[geometryIndex];

			geometries[geometryIndex] = {
				.flags = GeometryFlags::OPAQUE | GeometryFlags::NO_DUPLICATE_ANY_HIT_INVOCATION,
				.vertexView = vertexBuffer->createView<BuildVertex3D>(geometry.vertexRange.offset * sizeof(BuildVertex3D), geometry.vertexRange.count * sizeof(BuildVertex3D)),
				.vertexFormat = VertexInputAttributeFormat::R32G32B32_FLOAT,
				.indexView = indexBuffer->createView<uint32>(geometry.indexRange.offset * sizeof(uint32), geometry.indexRange.count * sizeof(uint32)),
			};

			geometryIndex += 1;
		}

		const ref<AccelerationStructureBuildGeometry> buildTriangleGeometry = device->createAccelerationStructureBuildGeometry({
			.flags = AccelerationStructureBuildFlags::PREFER_FAST_TRACE,
			.geometries = geometries,
		});

		const ref<AccelerationStructureBuildGeometry> buildInstanceGeometry = device->createAccelerationStructureBuildGeometry({
			.flags = AccelerationStructureBuildFlags::PREFER_FAST_TRACE,
			.geometryFlags = GeometryFlags::OPAQUE | GeometryFlags::NO_DUPLICATE_ANY_HIT_INVOCATION,
			.instanceView = instanceView,
			.instanceCount = instanceCount,
		});

		const AccelerationStructureBuildSizes bottomLevelAccelerationStructureBuildSizes = device->getAccelerationStructureBuildSizes(buildTriangleGeometry);
		const AccelerationStructureBuildSizes topLevelAccelerationStructureBuildSizes = device->getAccelerationStructureBuildSizes(buildInstanceGeometry);

		accelerationStructureBuffer = device->createStaticBuffer({
			.size = ceil(bottomLevelAccelerationStructureBuildSizes.accelerationStructureSize, 256ull) + ceil(topLevelAccelerationStructureBuildSizes.accelerationStructureSize, 256ull),
			.usage = BufferUsage::SHADER_DEVICE_ADDRESS | BufferUsage::ACCELERATION_STRUCTURE_STORAGE,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL,
			.persistent = false,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Acceleration structure",
			#endif
		});

		const ref<Buffer> scratchBuffer = device->createStaticBuffer({
			.size = ceil(bottomLevelAccelerationStructureBuildSizes.scratchBuildSize, scratchAlignment) + ceil(topLevelAccelerationStructureBuildSizes.scratchBuildSize, scratchAlignment),
			.usage = BufferUsage::READ_ONLY_STORAGE | BufferUsage::SHADER_DEVICE_ADDRESS,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL,
			.persistent = false,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Scratch",
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

		const uint32 memoryTypeIndex = memoryAllocator->findMemoryTypeIndex(MemoryProperty::DEVICE_LOCAL, false);
		const uint32 allocationIndex = memoryAllocator->allocate(false, memoryTypeIndex, allocationSize, allocationAlignment, true);

		memoryAllocator->suballocateStatic(allocationIndex, accelerationStructureBuffer);
		memoryAllocator->suballocateStatic(allocationIndex, scratchBuffer);

		const BufferView<uint8> bottomLevelAccelerationStructureView = accelerationStructureBuffer->createView<uint8>(0, ceil(bottomLevelAccelerationStructureBuildSizes.accelerationStructureSize, 256ull));
		const BufferView<uint8> topLevelAccelerationStructureView = accelerationStructureBuffer->createView<uint8>(bottomLevelAccelerationStructureView.getOffset() + bottomLevelAccelerationStructureView.getSize(), ceil(topLevelAccelerationStructureBuildSizes.accelerationStructureSize, 256ull));
		const BufferView<uint8> bottomLevelScratchView = scratchBuffer->createView<uint8>(0, ceil(bottomLevelAccelerationStructureBuildSizes.scratchBuildSize, scratchAlignment));
		const BufferView<uint8> topLevelScratchView = scratchBuffer->createView<uint8>(bottomLevelScratchView.getOffset() + bottomLevelScratchView.getSize(), ceil(topLevelAccelerationStructureBuildSizes.scratchBuildSize, scratchAlignment));
		const BufferView<AccelerationStructureInstance> instanceStagingView = instanceStagingBuffer->createView<AccelerationStructureInstance>(0, instanceStagingBuffer->getSize());

		bottomLevelAccelerationStructure = device->createAccelerationStructure({
			.view = bottomLevelAccelerationStructureView,
			.buildGeometry = buildTriangleGeometry,
		});
		topLevelAccelerationStructure = device->createAccelerationStructure({
			.view = topLevelAccelerationStructureView,
			.buildGeometry = buildInstanceGeometry,
		});

		const uint64 bottomLevelAccelerationStructureAddress = bottomLevelAccelerationStructure->getDeviceAddress();

		AccelerationStructureInstance* instanceMappedRange = instanceStagingView.map();
		uint32 instanceIndex = 0;

		while (instanceIndex < scene.staticGeometry.instances.count()) {
			const BuildTransform3D& transform = scene.staticGeometry.instances[instanceIndex];
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
			instance.accelerationStructureAddress = bottomLevelAccelerationStructureAddress;

			instanceIndex += 1;
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

		const BufferCopyRegion instanceCopyRegion = {
			.sourceOffset = instanceStagingView.getOffset(),
			.destinationOffset = instanceView.getOffset(),
			.size = instanceStagingView.getSize(),
		};

		commandEncoder->copyBufferToBuffer(instanceStagingBuffer, instanceBuffer, instanceCopyRegion);

		const BufferMemoryBarrier instanceMemoryBarrier = {
			.buffer = instanceBuffer,
			.offset = instanceView.getOffset(),
			.size = instanceView.getSize(),
			.newAccess = Access::ACCELERATION_STRUCTURE,
		};

		commandEncoder->insertMemoryBarriers(instanceMemoryBarrier, {});
		commandEncoder->buildAccelerationStructures(bottomLevelAccelerationStructure, bottomLevelAccelerationStructureView, bottomLevelScratchView);
		commandEncoder->buildAccelerationStructures(topLevelAccelerationStructure, topLevelAccelerationStructureView, topLevelScratchView);

		const ref<CommandBuffer> commandBuffer = commandEncoder->end();

		queue->submit(commandBuffer);
		queue->waitIdle();
	}
}