// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/VK/Buffer.hpp"
#include "Engine/Adapter/Graphics/VK/Device.hpp"
#include "Engine/Adapter/Graphics/VK/DeviceExtension/EXT_hardware_ray_tracing/AccelerationStructure.hpp"
#include "Engine/Adapter/Graphics/VK/DeviceExtension/EXT_hardware_ray_tracing/AccelerationStructureBuildFlags.hpp"
#include "Engine/Adapter/Graphics/VK/DeviceExtension/EXT_hardware_ray_tracing/AccelerationStructureBuildGeometry.hpp"
#include "Engine/Adapter/Graphics/VK/DeviceExtension/EXT_hardware_ray_tracing/Device.hpp"
#include "Engine/Adapter/Graphics/VK/DeviceExtension/EXT_hardware_ray_tracing/GeometryFlags.hpp"
#include "Engine/Adapter/Graphics/VK/IndexType.hpp"
#include "Engine/Adapter/Graphics/VK/VertexInputAttributeFormat.hpp"

namespace Trident::VK {
	ref<Trident::AccelerationStructure> createAccelerationStructure(const Trident::Device* device, const AccelerationStructureDescriptor& descriptor) {
		const VK::Device* vkDevice = static_cast<const VK::Device*>(device);
		const BaseBufferView& view = descriptor.view;
		const ref<VK::AccelerationStructureBuildGeometry> buildGeometry = ref_cast<VK::AccelerationStructureBuildGeometry>(descriptor.buildGeometry);

		ref<VK::AccelerationStructure> accelerationStructure = allocateRef<VK::AccelerationStructure>();
		accelerationStructure->device = device;
		accelerationStructure->buildGeometryInfo = buildGeometry->buildGeometryInfo;
		accelerationStructure->buildRangeInfos = &buildGeometry->buildRangeInfos[0];

		VkAccelerationStructureCreateInfoKHR accelerationStructureCreateInfo{};
		accelerationStructureCreateInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
		accelerationStructureCreateInfo.pNext = nullptr;
		accelerationStructureCreateInfo.createFlags = 0;
		accelerationStructureCreateInfo.buffer = pointer_cast<const VK::Buffer*>(view.getBuffer())->buffer;
		accelerationStructureCreateInfo.offset = view.getOffset();
		accelerationStructureCreateInfo.size = view.getSize();
		accelerationStructureCreateInfo.type = buildGeometry->buildGeometryInfo.type;
		accelerationStructureCreateInfo.deviceAddress = 0;

		const VkResult result = vkDevice->vkCreateAccelerationStructureKHR(vkDevice->device, &accelerationStructureCreateInfo, nullptr, &accelerationStructure->accelerationStructure);

		ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to create acceleration structure.");

		return accelerationStructure;
	}

	ref<Trident::AccelerationStructureBuildGeometry> createAccelerationStructureBuildInstanceGeometry(const Trident::Device* device, const AccelerationStructureBuildInstanceGeometryDescriptor& descriptor) {
		const VK::Device* vkDevice = static_cast<const VK::Device*>(device);

		ref<VK::AccelerationStructureBuildGeometry> accelerationStructureBuildGeometry = allocateRef<VK::AccelerationStructureBuildGeometry>();

		sequence<VkAccelerationStructureGeometryKHR>& geometries = accelerationStructureBuildGeometry->geometries;
		geometries = sequence<VkAccelerationStructureGeometryKHR>(1);

		sequence<VkAccelerationStructureBuildRangeInfoKHR>& buildRangeInfos = accelerationStructureBuildGeometry->buildRangeInfos;
		buildRangeInfos = sequence<VkAccelerationStructureBuildRangeInfoKHR>(1);

		sequence<uint32>& maxPrimitiveCounts = accelerationStructureBuildGeometry->maxPrimitiveCounts;
		maxPrimitiveCounts = sequence<uint32>(1);

		VkBufferDeviceAddressInfo bufferDeviceAddressInfo{};
		bufferDeviceAddressInfo.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
		bufferDeviceAddressInfo.pNext = nullptr;

		const BaseBufferView& instanceView = descriptor.instanceView;
		const uint32 instanceStride = instanceView.getStride();

		VkAccelerationStructureGeometryKHR& geometry = geometries[0];
		geometry.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
		geometry.pNext = nullptr;
		geometry.geometryType = VK_GEOMETRY_TYPE_INSTANCES_KHR;
		geometry.flags = translateGeometryFlags(descriptor.geometryFlags);

		bufferDeviceAddressInfo.buffer = pointer_cast<const VK::Buffer*>(instanceView.getBuffer())->buffer;

		VkAccelerationStructureGeometryInstancesDataKHR& instancesData = geometry.geometry.instances;
		instancesData.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_INSTANCES_DATA_KHR;
		instancesData.pNext = nullptr;
		instancesData.arrayOfPointers = false;
		instancesData.data.deviceAddress = vkDevice->vkGetBufferDeviceAddress(vkDevice->device, &bufferDeviceAddressInfo);

		VkAccelerationStructureBuildRangeInfoKHR& buildRangeInfo = buildRangeInfos[0];
		buildRangeInfo.primitiveCount = descriptor.instanceCount;
		buildRangeInfo.primitiveOffset = static_cast<uint32>(instanceView.getOffset());
		buildRangeInfo.firstVertex = 0;
		buildRangeInfo.transformOffset = 0;

		maxPrimitiveCounts[0] = buildRangeInfo.primitiveCount;

		VkAccelerationStructureBuildGeometryInfoKHR& buildGeometryInfo = accelerationStructureBuildGeometry->buildGeometryInfo;
		buildGeometryInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
		buildGeometryInfo.pNext = nullptr;
		buildGeometryInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
		buildGeometryInfo.flags = translateAccelerationStructureBuildFlags(descriptor.flags);

		// TODO(graphics/vk/ext_hardware_ray_tracing/build-mode): Expose the build mode?
		buildGeometryInfo.mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;

		buildGeometryInfo.srcAccelerationStructure = nullptr;
		buildGeometryInfo.dstAccelerationStructure = nullptr;
		buildGeometryInfo.geometryCount = 1;
		buildGeometryInfo.pGeometries = &geometries[0];
		buildGeometryInfo.ppGeometries = nullptr;
		buildGeometryInfo.scratchData.deviceAddress = 0;

		return accelerationStructureBuildGeometry;
	}

	ref<Trident::AccelerationStructureBuildGeometry> createAccelerationStructureBuildTriangleGeometry(const Trident::Device* device, const AccelerationStructureBuildTriangleGeometryDescriptor& descriptor) {
		const VK::Device* vkDevice = static_cast<const VK::Device*>(device);

		ref<VK::AccelerationStructureBuildGeometry> accelerationStructureBuildGeometry = allocateRef<VK::AccelerationStructureBuildGeometry>();

		sequence<VkAccelerationStructureGeometryKHR>& geometries = accelerationStructureBuildGeometry->geometries;
		geometries = sequence<VkAccelerationStructureGeometryKHR>(descriptor.geometries.count());

		sequence<VkAccelerationStructureBuildRangeInfoKHR>& buildRangeInfos = accelerationStructureBuildGeometry->buildRangeInfos;
		buildRangeInfos = sequence<VkAccelerationStructureBuildRangeInfoKHR>(descriptor.geometries.count());

		sequence<uint32>& maxPrimitiveCounts = accelerationStructureBuildGeometry->maxPrimitiveCounts;
		maxPrimitiveCounts = sequence<uint32>(descriptor.geometries.count());

		VkBufferDeviceAddressInfo bufferDeviceAddressInfo{};
		bufferDeviceAddressInfo.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
		bufferDeviceAddressInfo.pNext = nullptr;

		VkAccelerationStructureBuildGeometryInfoKHR& buildGeometryInfo = accelerationStructureBuildGeometry->buildGeometryInfo;
		buildGeometryInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
		buildGeometryInfo.pNext = nullptr;
		buildGeometryInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
		buildGeometryInfo.flags = translateAccelerationStructureBuildFlags(descriptor.flags);

		// TODO(graphics/vk/ext_hardware_ray_tracing/build-mode): Expose the build mode?
		buildGeometryInfo.mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;

		buildGeometryInfo.srcAccelerationStructure = nullptr;
		buildGeometryInfo.dstAccelerationStructure = nullptr;
		buildGeometryInfo.geometryCount = static_cast<uint32>(geometries.count());
		buildGeometryInfo.pGeometries = &geometries[0];
		buildGeometryInfo.ppGeometries = nullptr;
		buildGeometryInfo.scratchData.deviceAddress = 0;

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
			geometry.flags = translateGeometryFlags(triangleGeometryDescriptor.flags);

			VkAccelerationStructureGeometryTrianglesDataKHR& trianglesData = geometry.geometry.triangles;
			trianglesData.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR;
			trianglesData.pNext = nullptr;
			trianglesData.vertexFormat = getFormat(triangleGeometryDescriptor.vertexFormat);

			bufferDeviceAddressInfo.buffer = pointer_cast<const VK::Buffer*>(vertexView.getBuffer())->buffer;

			trianglesData.vertexData.deviceAddress = vkDevice->vkGetBufferDeviceAddress(vkDevice->device, &bufferDeviceAddressInfo);
			trianglesData.vertexStride = vertexStride;
			trianglesData.maxVertex = static_cast<uint32>(vertexView.getSize()) / vertexStride - 1;
			trianglesData.indexType = getIndexType(indexStride);

			bufferDeviceAddressInfo.buffer = pointer_cast<const VK::Buffer*>(indexView.getBuffer())->buffer;

			trianglesData.indexData.deviceAddress = vkDevice->vkGetBufferDeviceAddress(vkDevice->device, &bufferDeviceAddressInfo);
			trianglesData.transformData.deviceAddress = 0;

			VkAccelerationStructureBuildRangeInfoKHR& buildRangeInfo = buildRangeInfos[geometryIndex];
			buildRangeInfo.primitiveCount = static_cast<uint32>(indexView.getSize()) / (indexStride * 3);
			buildRangeInfo.primitiveOffset = static_cast<uint32>(indexView.getOffset());
			buildRangeInfo.firstVertex = static_cast<uint32>(vertexView.getOffset()) / vertexStride;
			buildRangeInfo.transformOffset = 0;

			maxPrimitiveCounts[geometryIndex] = buildRangeInfo.primitiveCount;

			geometryIndex += 1;
		}

		return accelerationStructureBuildGeometry;
	}

	AccelerationStructureBuildSizes getAccelerationStructureBuildSizes(const Trident::Device* device, ref<Trident::AccelerationStructureBuildGeometry> buildGeometry) {
		const VK::Device* vkDevice = static_cast<const VK::Device*>(device);
		const ref<VK::AccelerationStructureBuildGeometry> geometry = ref_cast<VK::AccelerationStructureBuildGeometry>(buildGeometry);

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