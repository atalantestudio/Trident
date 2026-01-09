// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/DX12/CommandBuffer.hpp"
#include "Engine/Adapter/Graphics/DX12/CommandEncoder.hpp"
#include "Engine/Adapter/Graphics/DX12/Queue.hpp"
#include "Engine/Adapter/Graphics/DX12/Swapchain.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace ProjectA {
	DX12::Queue::Queue(const DX12::Device& device, D3D12_COMMAND_LIST_TYPE commandListType) :
		device(device),
		commandPool(device, commandListType)
	{
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		D3D12_COMMAND_QUEUE_DESC commandQueueDescriptor{};
		commandQueueDescriptor.Type = commandListType;
		commandQueueDescriptor.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		commandQueueDescriptor.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		commandQueueDescriptor.NodeMask = 0;

		const HRESULT result = device.device->CreateCommandQueue(&commandQueueDescriptor, IID_PPV_ARGS(&commandQueue));

		ADAPTER_ASSERT(SUCCEEDED(result), "Failed to create command queue.");

		idleFence = createRef<DX12::Fence>(device, 0);
		idleEvent = CreateEvent(nullptr, false, false, nullptr);

		WIN32_ASSERT(idleEvent != nullptr);
	}

	DX12::Queue::~Queue() {
		const BOOL result = CloseHandle(idleEvent);

		WIN32_ASSERT(result != 0);
	}

	ref<ProjectA::CommandEncoder> DX12::Queue::createCommandEncoder(const CommandBufferDescriptor& descriptor) const {
		return createRef<DX12::CommandEncoder>(descriptor, device, commandPool);
	}

	void DX12::Queue::submit(const sequence<ref<ProjectA::CommandBuffer>>& commandBuffers, ref<Semaphore> waitSemaphore, ref<Semaphore> signalSemaphore, ref<ProjectA::Fence> fence) const {
		sequence<ID3D12CommandList*> commandLists(commandBuffers.count());
		uint32 commandListIndex = 0;

		while (commandListIndex < commandLists.count()) {
			commandLists[commandListIndex] = std::static_pointer_cast<DX12::CommandBuffer>(commandBuffers[commandListIndex])->commandList.Get();

			commandListIndex += 1;
		}

		commandQueue->ExecuteCommandLists(static_cast<UINT>(commandLists.count()), &commandLists[0]);

		// TODO
		/* if (fence != nullptr) {
			const HRESULT result = commandQueue->Signal(std::static_pointer_cast<DX12::Fence>(fence)->fence.Get(), 1);

			ADAPTER_ASSERT(SUCCEEDED(result), "Failed to add signal to fence.");
		} */
	}

	PresentStatus DX12::Queue::present(const sequence<ref<ProjectA::Swapchain>>& swapchains, ref<Semaphore> waitSemaphore, const sequence<uint32>& imageIndices) const {
		HRESULT result;

		for (const ref<ProjectA::Swapchain> swapchain : swapchains) {
			// TODO: Set correct swap interval.
			result = std::static_pointer_cast<DX12::Swapchain>(swapchain)->swapChain->Present(1, 0);

			ADAPTER_ASSERT(SUCCEEDED(result), "Failed to present.");
		}

		return PresentStatus::SUCCESS;
	}

	// See https://github.com/gfx-rs/gfx/issues/1383.
	void DX12::Queue::waitIdle() const {
		HRESULT result;

		result = commandQueue->Signal(idleFence->fence.Get(), 1);

		ADAPTER_ASSERT(SUCCEEDED(result), "Failed to add signal to queue idle fence.");

		result = idleFence->fence->SetEventOnCompletion(1, idleEvent);

		ADAPTER_ASSERT(SUCCEEDED(result), "Failed to set event on fence completion.");

		const DWORD code = WaitForSingleObject(idleEvent, INFINITE);

		ADAPTER_ASSERT(code == WAIT_OBJECT_0, "Failed to wait for event.");

		// TODO: Reset fence?
		/* result = idleFence->fence->Signal(0);

		ADAPTER_ASSERT(SUCCEEDED(result), "Failed to reset queue idle fence."); */
	}
}