// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Graphics/GraphicsAdapter.hpp"
#include "Engine/API/Graphics/Instance.hpp"

namespace Trident {
	Instance::Instance(const GraphicsAdapter* adapter) :
		adapter(adapter)
	{}

	Instance::~Instance() {
		adapter->destroyInstanceImplementation(this);
	}

	ref<Surface> Instance::createSurface(ref<PhysicalDevice> physicalDevice, const Window* window) const {
		return adapter->createSurfaceImplementation(this, physicalDevice, window);
	}

	ref<PhysicalDevice> Instance::selectPhysicalDevice(bool preferIntegratedGpu) const {
		return adapter->selectPhysicalDeviceImplementation(this, preferIntegratedGpu);
	}
}