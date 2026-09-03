// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/DX12/Device.hpp"
#include "Engine/Adapter/Graphics/DX12/DeviceExtension/EXT_hardware_ray_tracing/AccelerationStructure.hpp"

namespace Trident::DX12 {
	uint64 getAccelerationStructureDeviceAddress(const Trident::Device* device, const Trident::AccelerationStructure* accelerationStructure) {
		const DX12::Device* Device = static_cast<const DX12::Device*>(device);
		const DX12::AccelerationStructure* AccelerationStructure = static_cast<const DX12::AccelerationStructure*>(accelerationStructure);

		return AccelerationStructure->BuildDesc.DestAccelerationStructureData;
	}
}