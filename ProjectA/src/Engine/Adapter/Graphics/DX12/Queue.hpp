// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/DX12/CommandPool.hpp"
#include "Engine/Adapter/Graphics/DX12/Device.hpp"

#include "Engine/API/Rendering/Queue.hpp"

namespace ProjectA {
	class DX12::Queue final : public ProjectA::Queue {
		public:
			explicit Queue(const DX12::Device& device, D3D12_COMMAND_LIST_TYPE commandListType);

			ref<ProjectA::CommandEncoder> createCommandEncoder(const CommandBufferDescriptor& descriptor) const final override;

			void submit(const sequence<ref<ProjectA::CommandBuffer>>& commandBuffers, const ref<ProjectA::Semaphore>& waitSemaphore, const ref<ProjectA::Semaphore>& signalSemaphore, const ref<ProjectA::Fence>& fence) const final override;

			PresentStatus present(const sequence<ref<ProjectA::Swapchain>>& swapchains, const ref<ProjectA::Semaphore>& waitSemaphore, const sequence<uint32>& imageIndices) const final override;

		public:
			ComPtr<ID3D12CommandQueue> commandQueue;

		private:
			const DX12::Device& device;
			DX12::CommandPool commandPool;
	};
}