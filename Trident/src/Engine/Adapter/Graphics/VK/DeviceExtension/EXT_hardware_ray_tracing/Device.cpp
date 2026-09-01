// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/VK/Buffer.hpp"
#include "Engine/Adapter/Graphics/VK/Device.hpp"
#include "Engine/Adapter/Graphics/VK/DeviceExtension/EXT_hardware_ray_tracing/AccelerationStructureBuildFlags.hpp"
#include "Engine/Adapter/Graphics/VK/DeviceExtension/EXT_hardware_ray_tracing/AccelerationStructureType.hpp"
#include "Engine/Adapter/Graphics/VK/DeviceExtension/EXT_hardware_ray_tracing/Device.hpp"
#include "Engine/Adapter/Graphics/VK/DeviceExtension/EXT_hardware_ray_tracing/GeometryFlags.hpp"
#include "Engine/Adapter/Graphics/VK/IndexType.hpp"
#include "Engine/Adapter/Graphics/VK/VertexInputAttributeFormat.hpp"

namespace Trident::VK {
	AccelerationStructureBuildSizes getAccelerationStructureBuildSizesImplementation(const Trident::Device* device, const AccelerationStructureBuildGeometryDescriptor& descriptor) {
		const VK::Device* vkDevice = static_cast<const VK::Device*>(device);

		sequence<VkAccelerationStructureGeometryKHR> accelerationStructureGeometries(descriptor.geometries.count());
		uint32 geometryIndex = 0;

		while (geometryIndex < descriptor.geometries.count()) {
			const AccelerationStructureTriangleGeometryDescriptor& geometry = descriptor.geometries[geometryIndex];

			VkAccelerationStructureGeometryKHR& accelerationStructureGeometry = accelerationStructureGeometries[geometryIndex];
			accelerationStructureGeometry.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
			accelerationStructureGeometry.pNext = nullptr;
			accelerationStructureGeometry.geometryType = VK_GEOMETRY_TYPE_TRIANGLES_KHR;
			accelerationStructureGeometry.geometry.triangles.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR;
			accelerationStructureGeometry.geometry.triangles.pNext = nullptr;
			accelerationStructureGeometry.geometry.triangles.vertexFormat = getFormat(geometry.vertex.format);
			accelerationStructureGeometry.geometry.triangles.vertexData.deviceAddress = VkDeviceAddress(ref_cast<VK::Buffer>(geometry.vertex.buffer)->buffer);
			accelerationStructureGeometry.geometry.triangles.vertexData.hostAddress = nullptr;
			accelerationStructureGeometry.geometry.triangles.vertexStride = geometry.vertex.stride;
			accelerationStructureGeometry.geometry.triangles.maxVertex = geometry.vertex.count - 1;
			accelerationStructureGeometry.geometry.triangles.indexType = getIndexType(geometry.index.byteSize);
			accelerationStructureGeometry.geometry.triangles.indexData.deviceAddress = VkDeviceAddress(ref_cast<VK::Buffer>(geometry.index.buffer)->buffer);
			accelerationStructureGeometry.geometry.triangles.indexData.hostAddress = nullptr;
			accelerationStructureGeometry.geometry.triangles.transformData.deviceAddress = 0;
			accelerationStructureGeometry.geometry.triangles.transformData.hostAddress = nullptr;
			accelerationStructureGeometry.flags = translateGeometryFlags(geometry.flags);

			geometryIndex += 1;
		}

		VkAccelerationStructureBuildGeometryInfoKHR accelerationStructureBuildGeometryInfo{};
		accelerationStructureBuildGeometryInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
		accelerationStructureBuildGeometryInfo.pNext = nullptr;
		accelerationStructureBuildGeometryInfo.type = translateAccelerationStructureType(descriptor.type);
		accelerationStructureBuildGeometryInfo.flags = translateAccelerationStructureBuildFlags(descriptor.flags);

		// TODO(graphics/vk/ext_hardware_ray_tracing/build-mode): Expose the build mode?
		accelerationStructureBuildGeometryInfo.mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;

		accelerationStructureBuildGeometryInfo.srcAccelerationStructure = nullptr;
		accelerationStructureBuildGeometryInfo.dstAccelerationStructure = nullptr;
		accelerationStructureBuildGeometryInfo.geometryCount = static_cast<uint32>(accelerationStructureGeometries.count());

		// TODO(graphics/vk/ext_hardware_ray_tracing/geometry-pointers): Expose both members?
		accelerationStructureBuildGeometryInfo.pGeometries = &accelerationStructureGeometries[0];
		accelerationStructureBuildGeometryInfo.ppGeometries = nullptr;

		accelerationStructureBuildGeometryInfo.scratchData.deviceAddress = 0;
		accelerationStructureBuildGeometryInfo.scratchData.hostAddress = nullptr;

		// TODO(graphics/vk/ext_hardware_ray_tracing/primitive-counts): Expose.
		static constexpr uint32 maxPrimitiveCounts[] = {1};

		VkAccelerationStructureBuildSizesInfoKHR sizeInfo{};
		sizeInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR;
		sizeInfo.pNext = nullptr;

		vkDevice->vkGetAccelerationStructureBuildSizesKHR(vkDevice->device, VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR, &accelerationStructureBuildGeometryInfo, maxPrimitiveCounts, &sizeInfo);

		AccelerationStructureBuildSizes buildSizes{};
		buildSizes.accelerationStructureSize = sizeInfo.accelerationStructureSize;
		buildSizes.scratchBuildSize = sizeInfo.buildScratchSize;
		buildSizes.scratchUpdateSize = sizeInfo.updateScratchSize;

		return buildSizes;
	}
}