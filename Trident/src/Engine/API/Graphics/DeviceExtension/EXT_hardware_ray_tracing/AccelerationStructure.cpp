// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Graphics/Device.hpp"
#include "Engine/API/Graphics/DeviceExtension/EXT_hardware_ray_tracing/AccelerationStructure.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace Trident {
	AccelerationStructure::~AccelerationStructure() {
		device->adapter->destroyAccelerationStructureImplementation(device, this);
	}

	uint64 AccelerationStructure::getDeviceAddress() const {
		return device->adapter->getAccelerationStructureDeviceAddressImplementation(device, this);
	}
}