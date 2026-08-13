// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Graphics/Device.hpp"
#include "Engine/API/Graphics/PhysicalDevice.hpp"

namespace Trident {
	view<char8> PhysicalDevice::getName() const {
		return adapter->getPhysicalDeviceNameImplementation(this);
	}

	uint32 PhysicalDevice::getMemoryTypeCount() const {
		return adapter->getPhysicalDeviceMemoryTypeCountImplementation(this);
	}

	const PhysicalDeviceLimits& PhysicalDevice::getLimits() const {
		return limits;
	}

	ref<Device> PhysicalDevice::createDevice() const {
		ref<Device> device = adapter->createDeviceImplementation(adapter, this);
		device->memoryAllocator = adapter->createMemoryAllocatorImplementation(device);

		return device;
	}

	PhysicalDevice::PhysicalDevice(const GraphicsAdapter* adapter) :
		adapter(adapter)
	{}
}