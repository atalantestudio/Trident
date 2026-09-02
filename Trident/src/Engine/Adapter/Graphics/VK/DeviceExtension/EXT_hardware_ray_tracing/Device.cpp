// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/VK/Buffer.hpp"
#include "Engine/Adapter/Graphics/VK/Device.hpp"
#include "Engine/Adapter/Graphics/VK/DeviceExtension/EXT_hardware_ray_tracing/AccelerationStructure.hpp"
#include "Engine/Adapter/Graphics/VK/DeviceExtension/EXT_hardware_ray_tracing/AccelerationStructureBuildFlags.hpp"
#include "Engine/Adapter/Graphics/VK/DeviceExtension/EXT_hardware_ray_tracing/AccelerationStructureBuildTriangleGeometry.hpp"
#include "Engine/Adapter/Graphics/VK/DeviceExtension/EXT_hardware_ray_tracing/AccelerationStructureType.hpp"
#include "Engine/Adapter/Graphics/VK/DeviceExtension/EXT_hardware_ray_tracing/Device.hpp"
#include "Engine/Adapter/Graphics/VK/DeviceExtension/EXT_hardware_ray_tracing/GeometryFlags.hpp"
#include "Engine/Adapter/Graphics/VK/IndexType.hpp"
#include "Engine/Adapter/Graphics/VK/VertexInputAttributeFormat.hpp"

namespace Trident::VK {
	ref<Trident::AccelerationStructure> createAccelerationStructure(const Trident::Device* device, const AccelerationStructureDescriptor& descriptor) {
		const VK::Device* vkDevice = static_cast<const VK::Device*>(device);
		const BaseBufferView& view = descriptor.view;
		const ref<VK::AccelerationStructureBuildTriangleGeometry> buildTriangleGeometry = ref_cast<VK::AccelerationStructureBuildTriangleGeometry>(descriptor.buildTriangleGeometry);

		ref<VK::AccelerationStructure> accelerationStructure = allocateRef<VK::AccelerationStructure>();
		accelerationStructure->device = device;
		accelerationStructure->buildGeometryInfo = buildTriangleGeometry->buildGeometryInfo;
		accelerationStructure->buildRangeInfos = &buildTriangleGeometry->buildRangeInfos[0];

		VkAccelerationStructureCreateInfoKHR accelerationStructureCreateInfo{};
		accelerationStructureCreateInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
		accelerationStructureCreateInfo.pNext = nullptr;
		accelerationStructureCreateInfo.createFlags = 0;
		accelerationStructureCreateInfo.buffer = pointer_cast<const VK::Buffer*>(view.getBuffer())->buffer;
		accelerationStructureCreateInfo.offset = view.getOffset();
		accelerationStructureCreateInfo.size = view.getSize();
		accelerationStructureCreateInfo.type = accelerationStructure->buildGeometryInfo.type;
		accelerationStructureCreateInfo.deviceAddress = 0;

		const VkResult result = vkDevice->vkCreateAccelerationStructureKHR(vkDevice->device, &accelerationStructureCreateInfo, nullptr, &accelerationStructure->accelerationStructure);

		ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to create acceleration structure.");

		return accelerationStructure;
	}

	ref<Trident::AccelerationStructureBuildTriangleGeometry> createAccelerationStructureBuildTriangleGeometry(const Trident::Device* device, const AccelerationStructureBuildTriangleGeometryDescriptor& descriptor) {
		const VK::Device* vkDevice = static_cast<const VK::Device*>(device);

		ref<VK::AccelerationStructureBuildTriangleGeometry> accelerationStructureBuildTriangleGeometry = allocateRef<VK::AccelerationStructureBuildTriangleGeometry>();

		sequence<VkAccelerationStructureGeometryKHR>& geometries = accelerationStructureBuildTriangleGeometry->geometries;
		geometries = sequence<VkAccelerationStructureGeometryKHR>(descriptor.geometries.count());

		sequence<VkAccelerationStructureBuildRangeInfoKHR>& buildRangeInfos = accelerationStructureBuildTriangleGeometry->buildRangeInfos;
		buildRangeInfos = sequence<VkAccelerationStructureBuildRangeInfoKHR>(descriptor.geometries.count());

		sequence<uint32>& maxPrimitiveCounts = accelerationStructureBuildTriangleGeometry->maxPrimitiveCounts;
		maxPrimitiveCounts = sequence<uint32>(descriptor.geometries.count());

		VkBufferDeviceAddressInfo bufferDeviceAddressInfo{};
		bufferDeviceAddressInfo.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
		bufferDeviceAddressInfo.pNext = nullptr;

		uint32 geometryIndex = 0;

		while (geometryIndex < descriptor.geometries.count()) {
			const AccelerationStructureTriangleGeometryDescriptor& triangleGeometryDescriptor = descriptor.geometries[geometryIndex];
			const BaseBufferView& vertexView = triangleGeometryDescriptor.vertexView;
			const BaseBufferView& indexView = triangleGeometryDescriptor.indexView;
			const uint32 vertexStride = vertexView.getStride();
			const uint32 indexStride = indexView.getStride();

			VkAccelerationStructureGeometryKHR& geometry = geometries[geometryIndex];
			geometry.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
			geometry.pNext = nullptr;
			geometry.geometryType = VK_GEOMETRY_TYPE_TRIANGLES_KHR;
			geometry.geometry.triangles.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR;
			geometry.geometry.triangles.pNext = nullptr;
			geometry.geometry.triangles.vertexFormat = getFormat(triangleGeometryDescriptor.vertexFormat);

			bufferDeviceAddressInfo.buffer = pointer_cast<const VK::Buffer*>(vertexView.getBuffer())->buffer;

			geometry.geometry.triangles.vertexData.deviceAddress = vkDevice->vkGetBufferDeviceAddress(vkDevice->device, &bufferDeviceAddressInfo);
			geometry.geometry.triangles.vertexStride = vertexStride;
			geometry.geometry.triangles.maxVertex = static_cast<uint32>(vertexView.getSize()) / vertexStride - 1;
			geometry.geometry.triangles.indexType = getIndexType(indexStride);

			bufferDeviceAddressInfo.buffer = pointer_cast<const VK::Buffer*>(indexView.getBuffer())->buffer;

			geometry.geometry.triangles.indexData.deviceAddress = vkDevice->vkGetBufferDeviceAddress(vkDevice->device, &bufferDeviceAddressInfo);
			geometry.geometry.triangles.transformData.deviceAddress = 0;
			geometry.flags = translateGeometryFlags(triangleGeometryDescriptor.flags);

			VkAccelerationStructureBuildRangeInfoKHR& buildRangeInfo = buildRangeInfos[geometryIndex];
			buildRangeInfo.primitiveCount = static_cast<uint32>(indexView.getSize()) / (indexStride * 3);
			buildRangeInfo.primitiveOffset = static_cast<uint32>(indexView.getOffset());
			buildRangeInfo.firstVertex = static_cast<uint32>(vertexView.getOffset()) / vertexStride;
			buildRangeInfo.transformOffset = 0;

			maxPrimitiveCounts[geometryIndex] = buildRangeInfo.primitiveCount;

			geometryIndex += 1;
		}

		VkAccelerationStructureBuildGeometryInfoKHR& buildGeometryInfo = accelerationStructureBuildTriangleGeometry->buildGeometryInfo;
		buildGeometryInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
		buildGeometryInfo.pNext = nullptr;
		buildGeometryInfo.type = translateAccelerationStructureType(descriptor.type);
		buildGeometryInfo.flags = translateAccelerationStructureBuildFlags(descriptor.flags);

		// TODO(graphics/vk/ext_hardware_ray_tracing/build-mode): Expose the build mode?
		buildGeometryInfo.mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;

		buildGeometryInfo.srcAccelerationStructure = nullptr;
		buildGeometryInfo.dstAccelerationStructure = nullptr;
		buildGeometryInfo.geometryCount = static_cast<uint32>(geometries.count());
		buildGeometryInfo.pGeometries = &geometries[0];

		// TODO(graphics/vk/ext_hardware_ray_tracing/geometry-pointers): Expose both members?
		buildGeometryInfo.ppGeometries = nullptr;

		buildGeometryInfo.scratchData.deviceAddress = 0;

		return accelerationStructureBuildTriangleGeometry;
	}

	AccelerationStructureBuildSizes getAccelerationStructureBuildSizes(const Trident::Device* device, ref<Trident::AccelerationStructureBuildTriangleGeometry> buildTriangleGeometry) {
		const VK::Device* vkDevice = static_cast<const VK::Device*>(device);
		const ref<VK::AccelerationStructureBuildTriangleGeometry> geometry = ref_cast<VK::AccelerationStructureBuildTriangleGeometry>(buildTriangleGeometry);

		VkAccelerationStructureBuildSizesInfoKHR sizeInfo{};
		sizeInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR;
		sizeInfo.pNext = nullptr;

		vkDevice->vkGetAccelerationStructureBuildSizesKHR(vkDevice->device, VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR, &geometry->buildGeometryInfo, &geometry->maxPrimitiveCounts[0], &sizeInfo);

		AccelerationStructureBuildSizes buildSizes{};
		buildSizes.accelerationStructureSize = sizeInfo.accelerationStructureSize;
		buildSizes.scratchBuildSize = sizeInfo.buildScratchSize;
		buildSizes.scratchUpdateSize = sizeInfo.updateScratchSize;

		return buildSizes;
	}
}