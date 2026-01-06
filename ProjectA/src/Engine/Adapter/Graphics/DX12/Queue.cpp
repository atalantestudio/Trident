// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/DX12/CommandBuffer.hpp"
#include "Engine/Adapter/Graphics/DX12/CommandEncoder.hpp"
#include "Engine/Adapter/Graphics/DX12/Queue.hpp"

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

		idleFence = createRef<DX12::Fence>(device, false);
		idleEvent = CreateEvent(nullptr, false, false, nullptr);

		WIN32_ASSERT(idleEvent != nullptr);
	}

	ref<ProjectA::CommandEncoder> DX12::Queue::createCommandEncoder(const CommandBufferDescriptor& descriptor) const {
		return device.commandEncoder;
	}

	void DX12::Queue::submit(const sequence<ref<ProjectA::CommandBuffer>>& commandBuffers, const ref<ProjectA::Semaphore>& waitSemaphore, const ref<ProjectA::Semaphore>& signalSemaphore, const ref<ProjectA::Fence>& fence) const {
		sequence<ID3D12CommandList*> commandLists(commandBuffers.count());
		uint32 commandListIndex = 0;

		while (commandListIndex < commandLists.count()) {
			commandLists[commandListIndex] = std::static_pointer_cast<DX12::CommandBuffer>(commandBuffers[commandListIndex])->commandList.Get();

			commandListIndex += 1;
		}

		commandQueue->ExecuteCommandLists(static_cast<UINT>(commandLists.count()), &commandLists[0]);
	}

	PresentStatus DX12::Queue::present(const sequence<ref<ProjectA::Swapchain>>& swapchains, const ref<ProjectA::Semaphore>& waitSemaphore, const sequence<uint32>& imageIndices) const {
		ENGINE_ERROR_UNIMPLEMENTED();

		return PresentStatus::SUCCESS;
	}

	// See https://github.com/gfx-rs/gfx/issues/1383.
	void DX12::Queue::waitIdle() const {
		HRESULT result;

		result = commandQueue->Signal(idleFence->fence.Get(), 1);

		ADAPTER_ASSERT(SUCCEEDED(result), "Failed to add signal to command queue.");

		result = idleFence->fence->SetEventOnCompletion(1, idleEvent);

		ADAPTER_ASSERT(SUCCEEDED(result), "Failed to set event on fence completion.");

		const DWORD code = WaitForSingleObject(idleEvent, INFINITE);

		ADAPTER_ASSERT(code == WAIT_OBJECT_0 || code == WAIT_TIMEOUT, "Failed to idle wait.");
	}
}