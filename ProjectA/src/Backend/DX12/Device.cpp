// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Backend/DX12/Device.hpp"
#include "Backend/DX12/Queue.hpp"
#include "Backend/DX12/Swapchain.hpp"

#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace ProjectA {
	DX12::Device::Device(const DX12::Instance& instance) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		HRESULT result;

		result = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device));

		if (FAILED(result)) {
			ProjectA::GraphicsAdapter::logger.warning("Failed to create device using the primary display adapter. Falling back to a WARP adapter.");

			ComPtr<IDXGIAdapter> adapter;

			result = instance.factory->EnumWarpAdapter(IID_PPV_ARGS(&adapter));

			ADAPTER_ASSERT(SUCCEEDED(result), "Failed to enumerate WARP adapter.");

			result = D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device));

			ADAPTER_ASSERT(SUCCEEDED(result), "Failed to create device using the WARP adapter.");
		}

		queues[static_cast<uint32>(QueueFamilyCapability::GRAPHICS)] = createRef<DX12::Queue>(*this, D3D12_COMMAND_LIST_TYPE_DIRECT);
		queues[static_cast<uint32>(QueueFamilyCapability::COMPUTE)] = createRef<DX12::Queue>(*this, D3D12_COMMAND_LIST_TYPE_COMPUTE);
		queues[static_cast<uint32>(QueueFamilyCapability::TRANSFER)] = createRef<DX12::Queue>(*this, D3D12_COMMAND_LIST_TYPE_COPY);
	}

	DX12::Device::~Device() {
		ENGINE_ERROR_UNIMPLEMENTED();
	}

	AcquireNextImageStatus DX12::Device::acquireNextImage(const ref<ProjectA::Swapchain>& swapchain, const ref<Semaphore>& semaphore, uint32& imageIndex) const {
		imageIndex = std::static_pointer_cast<Swapchain>(swapchain)->swapChain->GetCurrentBackBufferIndex();

		ENGINE_ERROR_UNIMPLEMENTED();

		return AcquireNextImageStatus::SUCCESS;
	}

	sequence<ref<DescriptorSet>> DX12::Device::allocateDescriptorSets(const sequence<ref<DescriptorSetLayout>>& descriptorSetLayouts) {
		ENGINE_ERROR_UNIMPLEMENTED();

		return {};
	}

	ref<PhysicalDevice> DX12::Device::getPhysicalDevice() const {
		// TODO
		ENGINE_ERROR_UNIMPLEMENTED();

		return nullptr;
	}

	ref<ProjectA::Queue> DX12::Device::getQueue(QueueFamilyCapability queueFamilyCapability) const {
		return queues[static_cast<uint32>(queueFamilyCapability)];
	}

	void DX12::Device::resetFence(const ref<ProjectA::Fence>& fence) const {
		ENGINE_ERROR_UNIMPLEMENTED();
	}

	void DX12::Device::updateDescriptorSets(const sequence<WriteDescriptorSet>& writeDescriptorSets) const {
		ENGINE_ERROR_UNIMPLEMENTED();
	}

	void DX12::Device::waitForFence(const ref<ProjectA::Fence>& fence) const {
		ENGINE_ERROR_UNIMPLEMENTED();
	}

	void DX12::Device::waitIdle() const {
		ENGINE_ERROR_UNIMPLEMENTED();
	}
}