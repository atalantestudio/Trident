// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/VK/Buffer.hpp"
#include "Engine/Adapter/Graphics/VK/CommandEncoder.hpp"
#include "Engine/Adapter/Graphics/VK/Device.hpp"
#include "Engine/Adapter/Graphics/VK/DeviceExtension/EXT_hardware_ray_tracing/AccelerationStructure.hpp"
#include "Engine/Adapter/Graphics/VK/DeviceExtension/EXT_hardware_ray_tracing/CommandEncoder.hpp"
#include "Engine/Adapter/Graphics/VK/DeviceExtension/EXT_hardware_ray_tracing/RayTracingPipeline.hpp"

namespace Trident::VK {
	void bindRayTracingPipeline(Trident::CommandEncoder* commandEncoder, ref<Trident::RayTracingPipeline> pipeline) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const VK::Device* vkDevice = static_cast<const VK::Device*>(commandEncoder->device);
		const ref<VK::RayTracingPipeline> vkPipeline = ref_cast<VK::RayTracingPipeline>(pipeline);

		VK::CommandEncoder* vkCommandEncoder = static_cast<VK::CommandEncoder*>(commandEncoder);

		vkDevice->vkCmdBindPipeline(vkCommandEncoder->commandBuffer, VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR, vkPipeline->pipeline);

		vkCommandEncoder->pipelineBindPoint = VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR;
		vkCommandEncoder->commandCount += 1;
	}

	void buildAccelerationStructures(Trident::CommandEncoder* commandEncoder, view<ref<Trident::AccelerationStructure>> accelerationStructures, view<BaseBufferView> scratchViews) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const VK::Device* vkDevice = static_cast<const VK::Device*>(commandEncoder->device);
		const uint32 accelerationStructureCount = static_cast<uint32>(accelerationStructures.count());

		VK::CommandEncoder* vkCommandEncoder = static_cast<VK::CommandEncoder*>(commandEncoder);
		sequence<VkAccelerationStructureBuildGeometryInfoKHR> buildGeometryInfos(accelerationStructureCount);
		sequence<const VkAccelerationStructureBuildRangeInfoKHR*> buildRangeInfos(accelerationStructureCount);

		VkBufferDeviceAddressInfo bufferDeviceAddressInfo{};
		bufferDeviceAddressInfo.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
		bufferDeviceAddressInfo.pNext = nullptr;

		uint32 accelerationStructureIndex = 0;

		while (accelerationStructureIndex < accelerationStructureCount) {
			const BaseBufferView& scratchView = scratchViews[accelerationStructureIndex];

			bufferDeviceAddressInfo.buffer = pointer_cast<const VK::Buffer*>(scratchView.getBuffer())->buffer;

			ref<VK::AccelerationStructure> accelerationStructure = ref_cast<VK::AccelerationStructure>(accelerationStructures[accelerationStructureIndex]);

			VkAccelerationStructureBuildGeometryInfoKHR& buildGeometryInfo = buildGeometryInfos[accelerationStructureIndex];
			buildGeometryInfo = accelerationStructure->buildGeometryInfo;
			buildGeometryInfo.scratchData.deviceAddress = vkDevice->vkGetBufferDeviceAddress(vkDevice->device, &bufferDeviceAddressInfo) + scratchView.getOffset();

			buildRangeInfos[accelerationStructureIndex] = accelerationStructure->buildRangeInfos;

			accelerationStructureIndex += 1;
		}

		vkDevice->vkCmdBuildAccelerationStructuresKHR(vkCommandEncoder->commandBuffer, accelerationStructureCount, &buildGeometryInfos[0], &buildRangeInfos[0]);

		vkCommandEncoder->commandCount += 1;
	}

	void traceRays(Trident::CommandEncoder* commandEncoder, const DeviceAddressRegion& generationShaderBindingTable, const DeviceAddressRegion& missShaderBindingTable, const DeviceAddressRegion& hitShaderBindingTable, const DeviceAddressRegion& callableShaderBindingTable, Extent3D extent) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const VK::Device* vkDevice = static_cast<const VK::Device*>(commandEncoder->device);

		VK::CommandEncoder* vkCommandEncoder = static_cast<VK::CommandEncoder*>(commandEncoder);

		vkDevice->vkCmdTraceRaysKHR(
			vkCommandEncoder->commandBuffer,
			pointer_cast<const VkStridedDeviceAddressRegionKHR*>(&generationShaderBindingTable),
			pointer_cast<const VkStridedDeviceAddressRegionKHR*>(&missShaderBindingTable),
			pointer_cast<const VkStridedDeviceAddressRegionKHR*>(&hitShaderBindingTable),
			pointer_cast<const VkStridedDeviceAddressRegionKHR*>(&callableShaderBindingTable),
			extent.x,
			extent.y,
			extent.z
		);

		vkCommandEncoder->commandCount += 1;
	}
}