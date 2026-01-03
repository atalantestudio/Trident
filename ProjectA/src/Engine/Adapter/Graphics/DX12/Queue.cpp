// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/DX12/CommandEncoder.hpp"
#include "Engine/Adapter/Graphics/DX12/Queue.hpp"

#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace ProjectA {
	DX12::Queue::Queue(const DX12::Device& device, D3D12_COMMAND_LIST_TYPE commandListType) :
		device(device),
		commandPool(device, commandListType)
	{
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		HRESULT result;

		D3D12_COMMAND_QUEUE_DESC commandQueueDescriptor{};
		commandQueueDescriptor.Type = commandListType;
		commandQueueDescriptor.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		commandQueueDescriptor.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		commandQueueDescriptor.NodeMask = 0;

		result = device.device->CreateCommandQueue(&commandQueueDescriptor, IID_PPV_ARGS(&commandQueue));

		ADAPTER_ASSERT(SUCCEEDED(result), "Failed to create command queue.");
	}

	ref<ProjectA::CommandEncoder> DX12::Queue::createCommandEncoder(const CommandBufferDescriptor& descriptor) const {
		return createRef<DX12::CommandEncoder>(descriptor, device, commandPool);
	}

	void DX12::Queue::submit(const sequence<ref<ProjectA::CommandBuffer>>& commandBuffers, const ref<ProjectA::Semaphore>& waitSemaphore, const ref<ProjectA::Semaphore>& signalSemaphore, const ref<ProjectA::Fence>& fence) const {
		ENGINE_ERROR_UNIMPLEMENTED();
	}

	PresentStatus DX12::Queue::present(const sequence<ref<ProjectA::Swapchain>>& swapchains, const ref<ProjectA::Semaphore>& waitSemaphore, const sequence<uint32>& imageIndices) const {
		ENGINE_ERROR_UNIMPLEMENTED();

		return PresentStatus::SUCCESS;
	}
}