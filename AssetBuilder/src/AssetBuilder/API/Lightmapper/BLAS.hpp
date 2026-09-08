// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/DeviceExtension/EXT_hardware_ray_tracing/AccelerationStructureInstance.hpp"
#include "Engine/API/Graphics/Queue.hpp"

#include "AssetBuilder/API/Lightmapper/Lightmapper.hpp"

namespace Trident {
	void Lightmapper::createBottomLevelAccelerationStructures(const BuildScene3D& scene) {
		static constexpr uint64 blasBufferAlignment = 256ull;

		const uint32 geometryCount = scene.staticGeometry.geometryCount;
		const uint64 scratchAlignment = limits->minAccelerationStructureScratchOffsetAlignment;

		blas = sequence<ref<AccelerationStructure>>(geometryCount);
		blasScratchViews = sequence<BaseBufferView>(geometryCount);

		sequence<ref<AccelerationStructureBuildGeometry>> buildGeometries(geometryCount);
		sequence<AccelerationStructureBuildSizes> buildSizes(geometryCount);
		sequence<uint64> viewOffsets(geometryCount);
		sequence<uint64> scratchOffsets(geometryCount);
		uint64 blasBufferSize = 0;
		uint64 scratchBufferSize = 0;
		uint32 geometryIndex = 0;

		while (geometryIndex < geometryCount) {
			const BuildGeometry& geometry = scene.geometries[geometryIndex];

			const AccelerationStructureTriangleGeometryDescriptor triangleGeometryDescriptor = {
				.flags = GeometryFlags::OPAQUE | GeometryFlags::NO_DUPLICATE_ANY_HIT_INVOCATION,
				.vertexView = vertexBuffer->createView<LightmapVertex>(geometry.vertexRange.offset * sizeof(LightmapVertex), geometry.vertexRange.count * sizeof(LightmapVertex)),
				.vertexFormat = VertexInputAttributeFormat::R32G32B32_FLOAT,
				.indexView = indexBuffer->createView<uint32>(geometry.indexRange.offset * sizeof(uint32), geometry.indexRange.count * sizeof(uint32)),
			};

			buildGeometries[geometryIndex] = device->createAccelerationStructureBuildGeometry({
				.flags = AccelerationStructureBuildFlags::PREFER_FAST_TRACE,
				.geometries = triangleGeometryDescriptor,
			});

			buildSizes[geometryIndex] = device->getAccelerationStructureBuildSizes(buildGeometries[geometryIndex]);
			viewOffsets[geometryIndex] = ceil(blasBufferSize, blasBufferAlignment);
			scratchOffsets[geometryIndex] = ceil(scratchBufferSize, scratchAlignment);

			blasBufferSize = viewOffsets[geometryIndex] + ceil(buildSizes[geometryIndex].accelerationStructureSize, blasBufferAlignment);
			scratchBufferSize += scratchOffsets[geometryIndex] + ceil(buildSizes[geometryIndex].scratchBuildSize, scratchAlignment);

			geometryIndex += 1;
		}

		blasBuffer = device->createDynamicBuffer({
			.size = blasBufferSize,
			.usage = BufferUsage::SHADER_DEVICE_ADDRESS | BufferUsage::ACCELERATION_STRUCTURE_STORAGE,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL,
			.persistent = false,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "BLAS",
			#endif
		});

		blasScratchBuffer = device->createDynamicBuffer({
			.size = ceil(scratchBufferSize, limits->minAccelerationStructureScratchOffsetAlignment),
			.usage = BufferUsage::READ_ONLY_STORAGE | BufferUsage::SHADER_DEVICE_ADDRESS,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL,
			.persistent = false,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "BLAS scratch",
			#endif
		});

		geometryIndex = 0;

		while (geometryIndex < geometryCount) {
			const BufferView<uint8> view = blasBuffer->createView<uint8>(viewOffsets[geometryIndex], ceil(buildSizes[geometryIndex].accelerationStructureSize, blasBufferAlignment));

			blas[geometryIndex] = device->createAccelerationStructure({
				.view = view,
				.buildGeometry = buildGeometries[geometryIndex],
			});

			blasScratchViews[geometryIndex] = blasScratchBuffer->createView<uint8>(scratchOffsets[geometryIndex], buildSizes[geometryIndex].scratchBuildSize);

			geometryIndex += 1;
		}
	}
}