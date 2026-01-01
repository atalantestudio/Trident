// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Backend/DX12/Device.hpp"

namespace ProjectA::DX12 {
	Device::Device(const ref<Instance>& instance) {
		HRESULT result;

		result = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device));

		if (FAILED(result)) {
			ComPtr<IDXGIAdapter> adapter;

			instance->factory->EnumWarpAdapter(IID_PPV_ARGS(&adapter));

			result = D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device));

			ENGINE_ASSERT(SUCCEEDED(result), "Could not create WARP device.");
		}
	}

	Device::~Device() {}

	AcquireNextImageStatus Device::acquireNextImage(const ref<Swapchain>& swapchain, const ref<Semaphore>& semaphore, uint32& imageIndex) const {
		return AcquireNextImageStatus::SUCCESS;
	}

	sequence<ref<DescriptorSet>> Device::allocateDescriptorSets(const sequence<ref<DescriptorSetLayout>>& descriptorSetLayouts) {
		return {};
	}

	ref<PhysicalDevice> Device::getPhysicalDevice() const {
		return nullptr;
	}

	ref<Queue> Device::getQueue(QueueFamilyCapability queueFamilyCapability) const {
		return nullptr;
	}

	void Device::resetFence(const ref<Fence>& fence) const {}

	void Device::updateDescriptorSets(const sequence<WriteDescriptorSet>& writeDescriptorSets) const {}

	void Device::waitForFence(const ref<Fence>& fence) const {}

	void Device::waitIdle() const {}
}