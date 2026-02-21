// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/DX12/CommandPool.hpp"
#include "Engine/Adapter/Graphics/DX12/Device.hpp"
#include "Engine/Adapter/Graphics/DX12/Fence.hpp"
#include "Engine/API/Rendering/Queue.hpp"

namespace Trident {
	class DX12::Queue final : public Trident::Queue {
		public:
			explicit Queue(const DX12::Device* device, D3D12_COMMAND_LIST_TYPE commandListType);

			~Queue() final override;

			ref<Trident::CommandEncoder> createCommandEncoder(const CommandBufferDescriptor& descriptor) const final override;

			void submit(view<ref<Trident::CommandBuffer>> commandBuffers) const final override;
			void submit(view<ref<Trident::CommandBuffer>> commandBuffers, ref<Semaphore> waitSemaphore, ref<Semaphore> signalSemaphore, ref<Trident::Fence> fence) const final override;

			PresentStatus present(view<ref<Trident::Swapchain>> swapchains, view<uint32> presentableImageIndices, ref<Semaphore> waitSemaphore) const final override;

			void waitIdle() const final override;

		public:
			ID3D12CommandQueue* commandQueue;
			DX12::CommandPool commandPool;

		private:
			const DX12::Device* device;
			ref<DX12::Fence> idleFence;
			HANDLE completionEvent;
	};
}