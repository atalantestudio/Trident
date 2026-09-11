// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/VK/Buffer.hpp"

#if defined(DEBUG_OBJECT_NAME)
	#include "Engine/Adapter/Graphics/VK/Debug/ObjectName.hpp"
#endif

#include "Engine/Adapter/Graphics/VK/Device.hpp"
#include "Engine/Adapter/Graphics/VK/DeviceExtension/EXT_hardware_ray_tracing/AccelerationStructure.hpp"
#include "Engine/Adapter/Graphics/VK/DeviceExtension/EXT_hardware_ray_tracing/AccelerationStructureBuildFlags.hpp"
#include "Engine/Adapter/Graphics/VK/DeviceExtension/EXT_hardware_ray_tracing/AccelerationStructureBuildGeometry.hpp"
#include "Engine/Adapter/Graphics/VK/DeviceExtension/EXT_hardware_ray_tracing/Device.hpp"
#include "Engine/Adapter/Graphics/VK/DeviceExtension/EXT_hardware_ray_tracing/GeometryFlags.hpp"
#include "Engine/Adapter/Graphics/VK/DeviceExtension/EXT_hardware_ray_tracing/RayTracingPipeline.hpp"
#include "Engine/Adapter/Graphics/VK/IndexType.hpp"
#include "Engine/Adapter/Graphics/VK/PipelineLayout.hpp"
#include "Engine/Adapter/Graphics/VK/ShaderModule.hpp"
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

		const VkResult result = vkDevice->vkCreateAccelerationStructureKHR(vkDevice->device, &accelerationStructureCreateInfo, nullptr, &accelerationStructure->buildGeometryInfo.dstAccelerationStructure);

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

		const BaseBufferView& instanceView = descriptor.instanceView;
		const uint32 instanceStride = instanceView.getStride();

		VkBufferDeviceAddressInfo bufferDeviceAddressInfo{};
		bufferDeviceAddressInfo.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
		bufferDeviceAddressInfo.pNext = nullptr;
		bufferDeviceAddressInfo.buffer = pointer_cast<const VK::Buffer*>(instanceView.getBuffer())->buffer;

		VkAccelerationStructureGeometryKHR& geometry = geometries[0];
		geometry.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
		geometry.pNext = nullptr;
		geometry.geometryType = VK_GEOMETRY_TYPE_INSTANCES_KHR;
		geometry.flags = translateGeometryFlags(descriptor.geometryFlags);

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
			trianglesData.indexType = translateIndexFormat(triangleGeometryDescriptor.indexFormat);

			bufferDeviceAddressInfo.buffer = pointer_cast<const VK::Buffer*>(indexView.getBuffer())->buffer;

			trianglesData.indexData.deviceAddress = vkDevice->vkGetBufferDeviceAddress(vkDevice->device, &bufferDeviceAddressInfo);
			trianglesData.transformData.deviceAddress = 0;

			VkAccelerationStructureBuildRangeInfoKHR& buildRangeInfo = buildRangeInfos[geometryIndex];
			buildRangeInfo.primitiveCount = static_cast<uint32>(indexView.getSize()) / (getIndexSize(triangleGeometryDescriptor.indexFormat) * 3);
			buildRangeInfo.primitiveOffset = static_cast<uint32>(indexView.getOffset());
			buildRangeInfo.firstVertex = static_cast<uint32>(vertexView.getOffset()) / vertexStride;
			buildRangeInfo.transformOffset = 0;

			maxPrimitiveCounts[geometryIndex] = buildRangeInfo.primitiveCount;

			geometryIndex += 1;
		}

		return accelerationStructureBuildGeometry;
	}

	ref<Trident::RayTracingPipeline> createRayTracingPipeline(const Trident::Device* device, const RayTracingPipelineDescriptor& descriptor) {
		const VK::Device* vkDevice = static_cast<const VK::Device*>(device);
		const ref<VK::PipelineLayout> pipelineLayout = ref_cast<VK::PipelineLayout>(descriptor.layout);

		ref<VK::RayTracingPipeline> rayTracingPipeline = allocateRef<VK::RayTracingPipeline>();
		rayTracingPipeline->device = device;

		VkRayTracingShaderGroupCreateInfoKHR rayTracingShaderGroupCreateInfo{};
		rayTracingShaderGroupCreateInfo.sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
		rayTracingShaderGroupCreateInfo.pNext = nullptr;
		rayTracingShaderGroupCreateInfo.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
		rayTracingShaderGroupCreateInfo.generalShader = VK_SHADER_UNUSED_KHR; // VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR
		rayTracingShaderGroupCreateInfo.closestHitShader = VK_SHADER_UNUSED_KHR; // VK_RAY_TRACING_SHADER_GROUP_TYPE_TRIANGLES_HIT_GROUP_KHR
		rayTracingShaderGroupCreateInfo.anyHitShader = VK_SHADER_UNUSED_KHR; // VK_RAY_TRACING_SHADER_GROUP_TYPE_TRIANGLES_HIT_GROUP_KHR
		rayTracingShaderGroupCreateInfo.intersectionShader = VK_SHADER_UNUSED_KHR;
		rayTracingShaderGroupCreateInfo.pShaderGroupCaptureReplayHandle = nullptr;

		sequence<VkPipelineShaderStageCreateInfo> shaderStageCreateInfos(descriptor.shaderModules.count());
		sequence<VkRayTracingShaderGroupCreateInfoKHR> shaderGroupCreateInfos(descriptor.shaderModules.count());
		uint32 shaderModuleIndex = 0;

		while (shaderModuleIndex < descriptor.shaderModules.count()) {
			const ref<VK::ShaderModule> shaderModule = ref_cast<VK::ShaderModule>(descriptor.shaderModules[shaderModuleIndex]);

			shaderStageCreateInfos[shaderModuleIndex] = shaderModule->createInfo;

			VkRayTracingShaderGroupCreateInfoKHR& shaderGroupCreateInfo = shaderGroupCreateInfos[shaderModuleIndex];
			shaderGroupCreateInfo.sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
			shaderGroupCreateInfo.pNext = nullptr;
			shaderGroupCreateInfo.generalShader = VK_SHADER_UNUSED_KHR;
			shaderGroupCreateInfo.closestHitShader = VK_SHADER_UNUSED_KHR;
			shaderGroupCreateInfo.anyHitShader = VK_SHADER_UNUSED_KHR;
			shaderGroupCreateInfo.intersectionShader = VK_SHADER_UNUSED_KHR;
			shaderGroupCreateInfo.pShaderGroupCaptureReplayHandle = nullptr;

			switch (shaderModule->createInfo.stage) {
				case VK_SHADER_STAGE_RAYGEN_BIT_KHR: {
					shaderGroupCreateInfo.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
					shaderGroupCreateInfo.generalShader = shaderModuleIndex;

					break;
				}
				case VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR: {
					shaderGroupCreateInfo.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_TRIANGLES_HIT_GROUP_KHR;
					shaderGroupCreateInfo.closestHitShader = shaderModuleIndex;

					break;
				}
				case VK_SHADER_STAGE_MISS_BIT_KHR: {
					shaderGroupCreateInfo.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_TRIANGLES_HIT_GROUP_KHR;
					shaderGroupCreateInfo.generalShader = shaderModuleIndex;

					break;
				}
				default: {
					ADAPTER_ASSERT(false, "Unsupported shader stage.");

					break;
				}
			}

			shaderModuleIndex += 1;
		}

		VkRayTracingPipelineInterfaceCreateInfoKHR interfaceCreateInfo{};
		interfaceCreateInfo.sType = VK_STRUCTURE_TYPE_RAY_TRACING_PIPELINE_INTERFACE_CREATE_INFO_KHR;
		interfaceCreateInfo.pNext = nullptr;
		interfaceCreateInfo.maxPipelineRayPayloadSize = descriptor.maxRayPayloadSize;
		interfaceCreateInfo.maxPipelineRayHitAttributeSize = descriptor.maxRayAttributeSize;

		VkRayTracingPipelineCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_RAY_TRACING_PIPELINE_CREATE_INFO_KHR;
		createInfo.pNext = &interfaceCreateInfo;
		createInfo.flags = 0;
		createInfo.stageCount = static_cast<uint32>(shaderStageCreateInfos.count());
		createInfo.pStages = &shaderStageCreateInfos[0];
		createInfo.groupCount = static_cast<uint32>(shaderGroupCreateInfos.count());
		createInfo.pGroups = &shaderGroupCreateInfos[0];
		createInfo.maxPipelineRayRecursionDepth = descriptor.maxRayRecursionDepth;
		createInfo.pLibraryInfo = nullptr;
		createInfo.pLibraryInterface = nullptr;
		createInfo.pDynamicState = nullptr;
		createInfo.layout = pipelineLayout->pipelineLayout;
		createInfo.basePipelineHandle = VK_NULL_HANDLE;
		createInfo.basePipelineIndex = -1;

		const VkResult result = vkDevice->vkCreateRayTracingPipelinesKHR(vkDevice->device, VK_NULL_HANDLE, VK_NULL_HANDLE, 1, &createInfo, nullptr, &rayTracingPipeline->pipeline);

		ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to create ray tracing pipeline.");

		#if defined(DEBUG_OBJECT_NAME)
			setObjectName(vkDevice, (uint64) rayTracingPipeline->pipeline, VK_OBJECT_TYPE_PIPELINE, descriptor.name);
		#endif

		return rayTracingPipeline;
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

	uint64 getBufferDeviceAddress(const Trident::Device* device, ref<Trident::Buffer> buffer) {
		const VK::Device* vkDevice = static_cast<const VK::Device*>(device);
		const ref<VK::Buffer> vkBuffer = ref_cast<VK::Buffer>(buffer);

		VkBufferDeviceAddressInfo info{};
		info.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
		info.pNext = nullptr;
		info.buffer = vkBuffer->buffer;

		return vkDevice->vkGetBufferDeviceAddress(vkDevice->device, &info);
	}
}