// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/VK/Buffer.hpp"
#include "Engine/Adapter/Graphics/VK/CommandEncoder.hpp"
#include "Engine/Adapter/Graphics/VK/Device.hpp"
#include "Engine/Adapter/Graphics/VK/DeviceExtension/EXT_hardware_ray_tracing/AccelerationStructure.hpp"
#include "Engine/Adapter/Graphics/VK/DeviceExtension/EXT_hardware_ray_tracing/CommandEncoder.hpp"

namespace Trident::VK {
	void buildAccelerationStructures(Trident::CommandEncoder* commandEncoder, view<ref<Trident::AccelerationStructure>> accelerationStructures, view<BaseBufferView> accelerationStructureViews, view<BaseBufferView> scratchViews) {
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
			buildGeometryInfo.dstAccelerationStructure = accelerationStructure->accelerationStructure;
			buildGeometryInfo.scratchData.deviceAddress = vkDevice->vkGetBufferDeviceAddress(vkDevice->device, &bufferDeviceAddressInfo);

			buildRangeInfos[accelerationStructureIndex] = accelerationStructure->buildRangeInfos;

			accelerationStructureIndex += 1;
		}

		vkDevice->vkCmdBuildAccelerationStructuresKHR(vkCommandEncoder->commandBuffer, accelerationStructureCount, &buildGeometryInfos[0], &buildRangeInfos[0]);

		vkCommandEncoder->commandCount += 1;
	}
}