// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/DX12/Device.hpp"
#include "Engine/Adapter/Graphics/DX12/Queue.hpp"
#include "Engine/Adapter/Graphics/DX12/Swapchain.hpp"

#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace ProjectA {
	DX12::Device::Device(const DX12::Instance& instance) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		HRESULT result = -1;

		#ifndef PREFER_SOFTWARE_ADAPTER
			result = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device));
		#endif

		if (FAILED(result)) {
			#ifndef PREFER_SOFTWARE_ADAPTER
				ProjectA::GraphicsAdapter::logger.warning("Failed to create device using the primary display adapter. Falling back to a WARP adapter.");
			#endif

			ComPtr<IDXGIAdapter> adapter;

			result = instance.factory->EnumWarpAdapter(IID_PPV_ARGS(&adapter));

			ADAPTER_ASSERT(SUCCEEDED(result), "Failed to enumerate WARP adapter.");

			result = D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device));

			ADAPTER_ASSERT(SUCCEEDED(result), "Failed to create device using the WARP adapter.");
		}

		queues[0] = createRef<DX12::Queue>(*this, D3D12_COMMAND_LIST_TYPE_DIRECT);
		queues[1] = createRef<DX12::Queue>(*this, D3D12_COMMAND_LIST_TYPE_COMPUTE);
		queues[2] = createRef<DX12::Queue>(*this, D3D12_COMMAND_LIST_TYPE_COPY);

		fenceEvent = CreateEvent(nullptr, false, false, nullptr);

		WIN32_ASSERT(fenceEvent != nullptr);
	}

	DX12::Device::~Device() {
		const BOOL result = CloseHandle(fenceEvent);

		WIN32_ASSERT(result != 0);
	}

	AcquireNextImageStatus DX12::Device::acquireNextImage(const ref<ProjectA::Swapchain>& swapchain, const ref<ProjectA::Semaphore>& semaphore, uint32& imageIndex) const {
		imageIndex = std::static_pointer_cast<DX12::Swapchain>(swapchain)->swapChain->GetCurrentBackBufferIndex();

		return AcquireNextImageStatus::SUCCESS;
	}

	sequence<ref<DescriptorSet>> DX12::Device::allocateDescriptorSets(const sequence<ref<DescriptorSetLayout>>& descriptorSetLayouts) {
		ENGINE_ERROR_UNIMPLEMENTED();

		return {};
	}

	ref<PhysicalDevice> DX12::Device::getPhysicalDevice() const {
		ENGINE_ERROR_UNIMPLEMENTED();

		return nullptr;
	}

	ref<ProjectA::Queue> DX12::Device::getQueue(QueueFamilyCapability queueFamilyCapability) const {
		return queues[static_cast<uint32>(queueFamilyCapability)];
	}

	void DX12::Device::resetFence(const ref<ProjectA::Fence>& fence) const {
		const HRESULT result = std::static_pointer_cast<DX12::Fence>(fence)->fence->Signal(0);

		ADAPTER_ASSERT(SUCCEEDED(result), "Failed to add signal to fence.");
	}

	void DX12::Device::updateDescriptorSets(const sequence<WriteDescriptorSet>& writeDescriptorSets) const {
		ENGINE_ERROR_UNIMPLEMENTED();
	}

	void DX12::Device::waitForFence(const ref<ProjectA::Fence>& fence) const {
		// TODO: Move this inside Queue::submit().
		const ref<DX12::Queue> queue = std::static_pointer_cast<DX12::Queue>(getQueue(QueueFamilyCapability::GRAPHICS));

		HRESULT result;

		result = queue->commandQueue->Signal(std::static_pointer_cast<DX12::Fence>(fence)->fence.Get(), 1);

		ADAPTER_ASSERT(SUCCEEDED(result), "Failed to add signal to fence.");

		result = ResetEvent(fenceEvent);

		ADAPTER_ASSERT(SUCCEEDED(result), "Failed to reset event.");

		result = std::static_pointer_cast<DX12::Fence>(fence)->fence->SetEventOnCompletion(1, fenceEvent);

		ADAPTER_ASSERT(SUCCEEDED(result), "Failed to set event on fence completion.");

		const DWORD code = WaitForSingleObject(fenceEvent, INFINITE);

		ADAPTER_ASSERT(code == WAIT_OBJECT_0, "Failed to wait for event.");
	}

	void DX12::Device::waitIdle() const {
		for (const ref<ProjectA::Queue>& queue : queues) {
			queue->waitIdle();
		}
	}
}