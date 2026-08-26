// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/VK/Buffer.hpp"
#include "Engine/Adapter/Graphics/VK/DeviceExtension/EXT_hardware_ray_tracing/BLASTriangleGeometry.hpp"

namespace Trident::VK {
	void testCreateBLASTriangleGeometryImplementation(const Trident::Device* device, const BLASTriangleGeometryDescriptor& descriptor) {
		const ref<VK::Buffer> vertexBuffer = ref_cast<VK::Buffer>(descriptor.vertexBuffer);
		const ref<VK::Buffer> indexBuffer = ref_cast<VK::Buffer>(descriptor.indexBuffer);

		VkAccelerationStructureGeometryKHR accelerationStructureGeometry{};
		accelerationStructureGeometry.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
		accelerationStructureGeometry.pNext = nullptr;
		accelerationStructureGeometry.geometryType = VK_GEOMETRY_TYPE_TRIANGLES_KHR;
		accelerationStructureGeometry.geometry.triangles.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR;
		accelerationStructureGeometry.geometry.triangles.pNext = nullptr;
		accelerationStructureGeometry.geometry.triangles.vertexFormat = VK_FORMAT_R32G32B32_SFLOAT;
		accelerationStructureGeometry.geometry.triangles.vertexData.deviceAddress = VkDeviceAddress(vertexBuffer->buffer);
		// accelerationStructureGeometry.geometry.triangles.vertexData.hostAddress;
		accelerationStructureGeometry.geometry.triangles.vertexStride = sizeof(svec3<float32>);
		// accelerationStructureGeometry.geometry.triangles.maxVertex;
		accelerationStructureGeometry.geometry.triangles.indexType = VK_INDEX_TYPE_UINT32;
		accelerationStructureGeometry.geometry.triangles.indexData.deviceAddress = VkDeviceAddress(indexBuffer->buffer);
		// accelerationStructureGeometry.geometry.triangles.indexData.hostAddress;
		// accelerationStructureGeometry.geometry.triangles.transformData.deviceAddress;
		// accelerationStructureGeometry.geometry.triangles.transformData.hostAddress;
		// accelerationStructureGeometry.geometry.aabbs;
		// accelerationStructureGeometry.geometry.instances;
		accelerationStructureGeometry.flags = VK_GEOMETRY_OPAQUE_BIT_KHR | VK_GEOMETRY_NO_DUPLICATE_ANY_HIT_INVOCATION_BIT_KHR;

		VkAccelerationStructureBuildRangeInfoKHR accelerationStructureBuildRangeInfo{};
		// accelerationStructureBuildRangeInfo.primitiveCount;
		accelerationStructureBuildRangeInfo.primitiveOffset = 0;
		accelerationStructureBuildRangeInfo.firstVertex = 0;
		accelerationStructureBuildRangeInfo.transformOffset = 0;
	}
}