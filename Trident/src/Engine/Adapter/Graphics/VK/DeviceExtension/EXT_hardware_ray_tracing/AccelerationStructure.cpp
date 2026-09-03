// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/VK/Device.hpp"
#include "Engine/Adapter/Graphics/VK/DeviceExtension/EXT_hardware_ray_tracing/AccelerationStructure.hpp"

namespace Trident::VK {
	void destroyAccelerationStructure(const Trident::Device* device, const Trident::AccelerationStructure* accelerationStructure) {
		const VK::Device* vkDevice = static_cast<const VK::Device*>(device);
		const VK::AccelerationStructure* vkAccelerationStructure = static_cast<const VK::AccelerationStructure*>(accelerationStructure);

		vkDevice->vkDestroyAccelerationStructureKHR(vkDevice->device, vkAccelerationStructure->accelerationStructure, nullptr);
	}

	uint64 getAccelerationStructureDeviceAddress(const Trident::Device* device, const Trident::AccelerationStructure* accelerationStructure) {
		const VK::Device* vkDevice = static_cast<const VK::Device*>(device);
		const VK::AccelerationStructure* vkAccelerationStructure = static_cast<const VK::AccelerationStructure*>(accelerationStructure);

		VkAccelerationStructureDeviceAddressInfoKHR deviceAddressInfo{};
		deviceAddressInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR;
		deviceAddressInfo.pNext = nullptr;
		deviceAddressInfo.accelerationStructure = vkAccelerationStructure->accelerationStructure;

		return vkDevice->vkGetAccelerationStructureDeviceAddressKHR(vkDevice->device, &deviceAddressInfo);
	}
}