// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/DX12/CommandBuffer.hpp"
#include "Engine/Adapter/Graphics/DX12/CommandEncoder.hpp"
#include "Engine/Adapter/Graphics/DX12/CommandListType.hpp"
#include "Engine/Adapter/Graphics/DX12/Queue.hpp"
#include "Engine/Adapter/Graphics/DX12/Swapchain.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace Trident {
	DX12::Queue::Queue(const DX12::Device* device, uint32 queueFamilyIndex, QueueFamilyCapability queueFamilyCapability) :
		Trident::Queue(device, queueFamilyIndex, queueFamilyCapability)
	{}

	ref<Trident::Queue> DX12::createQueue(const Trident::Device* device, uint32 queueFamilyIndex, QueueFamilyCapability queueFamilyCapability) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const DX12::Device* _device = static_cast<const DX12::Device*>(device);
		ref<DX12::Queue> queue = createRef<DX12::Queue>(_device, queueFamilyIndex, queueFamilyCapability);
		HRESULT result;

		D3D12_COMMAND_QUEUE_DESC commandQueueDescriptor{};
		commandQueueDescriptor.Type = getCommandListType(queueFamilyCapability);
		commandQueueDescriptor.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		commandQueueDescriptor.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		commandQueueDescriptor.NodeMask = 0;

		{
			PROFILE_SCOPE("CreateCommandQueue");

			result = _device->device->CreateCommandQueue(&commandQueueDescriptor, IID_PPV_ARGS(&queue->commandQueue));
		}

		ADAPTER_ASSERT(SUCCEEDED(result), "Failed to create command queue.");

		FenceDescriptor idleFenceDescriptor{};
		idleFenceDescriptor.signaled = false;

		SET_DEBUG_NAME(idleFenceDescriptor, "Queue idle fence");

		queue->idleFence = createFence(device, idleFenceDescriptor);
		queue->completionEvent = CreateEvent(nullptr, false, false, L"Completion event");

		WIN32_ASSERT(queue->completionEvent != nullptr);

		return queue;
	}

	void DX12::destroyQueue(const Trident::Queue* queue) {
		const DX12::Queue* _queue = static_cast<const DX12::Queue*>(queue);
		HRESULT result;

		result = CloseHandle(_queue->completionEvent);

		WIN32_ASSERT(result != 0);

		_queue->commandQueue->Release();
	}

	PresentStatus DX12::present(const Trident::Queue* queue, view<ref<Trident::Swapchain>> swapchains, view<uint32> presentableImageIndices, ref<Semaphore> waitSemaphore) {
		ADAPTER_ASSERT(swapchains.count() == presentableImageIndices.count(), "There must be 1 presentable image index per swapchain.");

		HRESULT result;

		for (const ref<Trident::Swapchain> swapchain : swapchains) {
			// TODO: Set correct swap interval.
			result = std::static_pointer_cast<DX12::Swapchain>(swapchain)->swapChain->Present(1, 0);

			ADAPTER_ASSERT(SUCCEEDED(result), "Failed to present.");
		}

		return PresentStatus::SUCCESS;
	}

	void DX12::submit(const Trident::Queue* queue, view<ref<Trident::CommandBuffer>> commandBuffers) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const DX12::Queue* _queue = static_cast<const DX12::Queue*>(queue);
		sequence<ID3D12CommandList*> commandLists(commandBuffers.count());
		UINT commandListIndex = 0;

		while (commandListIndex < commandLists.count()) {
			commandLists[commandListIndex] = std::static_pointer_cast<DX12::CommandBuffer>(commandBuffers[commandListIndex])->commandList;

			commandListIndex += 1;
		}

		_queue->commandQueue->ExecuteCommandLists(static_cast<UINT>(commandLists.count()), &commandLists[0]);
	}

	void DX12::submit(const Trident::Queue* queue, view<ref<Trident::CommandBuffer>> commandBuffers, ref<Semaphore> waitSemaphore, ref<Semaphore> signalSemaphore, ref<Trident::Fence> fence) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const DX12::Queue* _queue = static_cast<const DX12::Queue*>(queue);
		sequence<ID3D12CommandList*> commandLists(commandBuffers.count());
		HRESULT result;
		UINT commandListIndex = 0;

		while (commandListIndex < commandLists.count()) {
			commandLists[commandListIndex] = std::static_pointer_cast<DX12::CommandBuffer>(commandBuffers[commandListIndex])->commandList;

			commandListIndex += 1;
		}

		_queue->commandQueue->ExecuteCommandLists(static_cast<UINT>(commandLists.count()), &commandLists[0]);

		DEBUG_ASSERT(fence != nullptr);

		const ref<DX12::Fence> _fence = std::static_pointer_cast<DX12::Fence>(fence);

		result = _queue->commandQueue->Signal(_fence->fence, 1);

		ADAPTER_ASSERT(SUCCEEDED(result), "Failed to add signal to fence.");

		result = _fence->fence->SetEventOnCompletion(1, _queue->completionEvent);

		ADAPTER_ASSERT(SUCCEEDED(result), "Failed to set event on fence completion.");

		const DWORD code = WaitForSingleObject(_queue->completionEvent, INFINITE);

		WIN32_ASSERT(code == WAIT_OBJECT_0);
	}

	void DX12::waitIdleQueue(const Trident::Queue* queue) {
		const DX12::Queue* _queue = static_cast<const DX12::Queue*>(queue);
		const DX12::Fence* idleFence = static_cast<const DX12::Fence*>(_queue->idleFence.get());
		HRESULT result;

		// See https://github.com/gfx-rs/gfx/issues/1383.
		// TODO: We may not even need the idle fence if the completion event is set right after submit().
		result = _queue->commandQueue->Signal(idleFence->fence, 1);

		ADAPTER_ASSERT(SUCCEEDED(result), "Failed to add signal to queue idle fence.");

		result = idleFence->fence->SetEventOnCompletion(1, _queue->completionEvent);

		ADAPTER_ASSERT(SUCCEEDED(result), "Failed to set event on fence completion.");

		const DWORD code = WaitForSingleObject(_queue->completionEvent, INFINITE);

		WIN32_ASSERT(code == WAIT_OBJECT_0);

		result = idleFence->fence->Signal(0);

		ADAPTER_ASSERT(SUCCEEDED(result), "Failed to reset queue idle fence.");
	}
}