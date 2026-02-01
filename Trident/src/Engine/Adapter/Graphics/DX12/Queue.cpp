// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/DX12/CommandBuffer.hpp"
#include "Engine/Adapter/Graphics/DX12/CommandEncoder.hpp"
#include "Engine/Adapter/Graphics/DX12/Queue.hpp"
#include "Engine/Adapter/Graphics/DX12/Swapchain.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace Trident {
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

		FenceDescriptor idleFenceDescriptor{};
		idleFenceDescriptor.signaled = false;

		SET_DEBUG_NAME(idleFenceDescriptor, "Queue idle fence");

		idleFence = createRef<DX12::Fence>(idleFenceDescriptor, device);
		completionEvent = CreateEvent(nullptr, false, false, L"Completion event");

		WIN32_ASSERT(completionEvent != nullptr);
	}

	DX12::Queue::~Queue() {
		HRESULT result;

		result = CloseHandle(completionEvent);

		WIN32_ASSERT(result != 0);

		commandQueue->Release();
	}

	ref<Trident::CommandEncoder> DX12::Queue::createCommandEncoder(const CommandBufferDescriptor& descriptor) const {
		return createRef<DX12::CommandEncoder>(descriptor, device, commandPool);
	}

	void DX12::Queue::submit(view<ref<Trident::CommandBuffer>> commandBuffers, ref<Semaphore> waitSemaphore, ref<Semaphore> signalSemaphore, ref<Trident::Fence> fence) const {
		sequence<ID3D12CommandList*> commandLists(commandBuffers.count());
		UINT commandListIndex = 0;
		HRESULT result;

		while (commandListIndex < commandLists.count()) {
			commandLists[commandListIndex] = std::static_pointer_cast<DX12::CommandBuffer>(commandBuffers[commandListIndex])->commandList;

			commandListIndex += 1;
		}

		commandQueue->ExecuteCommandLists(static_cast<UINT>(commandLists.count()), &commandLists[0]);

		if (fence != nullptr) {
			const ref<DX12::Fence> _fence = std::static_pointer_cast<DX12::Fence>(fence);

			result = commandQueue->Signal(_fence->fence, 1);

			ADAPTER_ASSERT(SUCCEEDED(result), "Failed to add signal to fence.");

			result = _fence->fence->SetEventOnCompletion(1, completionEvent);

			ADAPTER_ASSERT(SUCCEEDED(result), "Failed to set event on fence completion.");

			const DWORD code = WaitForSingleObject(completionEvent, INFINITE);

			WIN32_ASSERT(code == WAIT_OBJECT_0);
		}
	}

	PresentStatus DX12::Queue::present(view<ref<Trident::Swapchain>> swapchains, view<uint32> presentableImageIndices, ref<Semaphore> waitSemaphore) const {
		ADAPTER_ASSERT(swapchains.count() == presentableImageIndices.count(), "There must be 1 presentable image index per swapchain.");

		HRESULT result;

		for (const ref<Trident::Swapchain> swapchain : swapchains) {
			// TODO: Set correct swap interval.
			result = std::static_pointer_cast<DX12::Swapchain>(swapchain)->swapChain->Present(1, 0);

			ADAPTER_ASSERT(SUCCEEDED(result), "Failed to present.");
		}

		return PresentStatus::SUCCESS;
	}

	// See https://github.com/gfx-rs/gfx/issues/1383.
	void DX12::Queue::waitIdle() const {
		HRESULT result;

		// TODO: We may not even need the idle fence if the completion event is set right after submit().
		result = commandQueue->Signal(idleFence->fence, 1);

		ADAPTER_ASSERT(SUCCEEDED(result), "Failed to add signal to queue idle fence.");

		result = idleFence->fence->SetEventOnCompletion(1, completionEvent);

		ADAPTER_ASSERT(SUCCEEDED(result), "Failed to set event on fence completion.");

		const DWORD code = WaitForSingleObject(completionEvent, INFINITE);

		WIN32_ASSERT(code == WAIT_OBJECT_0);

		result = idleFence->fence->Signal(0);

		ADAPTER_ASSERT(SUCCEEDED(result), "Failed to reset queue idle fence.");
	}
}