// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/DX12/CommandPool.hpp"
#include "Engine/Adapter/Graphics/DX12/Device.hpp"
#include "Engine/Adapter/Graphics/DX12/Fence.hpp"
#include "Engine/API/Graphics/Queue.hpp"

namespace Trident {
	class DX12::Queue final : public Trident::Queue {
		public:
			explicit Queue(const DX12::Device* device, uint32 queueFamilyIndex, QueueFamilyCapability queueFamilyCapability);

		public:
			ID3D12CommandQueue* commandQueue;
			ref<Trident::Fence> idleFence;
			HANDLE completionEvent;
	};

	namespace DX12 {
		ref<Trident::Queue> createQueue(const Trident::Device* device, uint32 queueFamilyIndex, QueueFamilyCapability queueFamilyCapability);

		void destroyQueue(const Trident::Queue* queue);

		PresentStatus present(const Trident::Queue* queue, view<ref<Trident::Swapchain>> swapchains, view<uint32> presentableImageIndices, ref<Semaphore> waitSemaphore);

		void submit(const Trident::Queue* queue, view<ref<Trident::CommandBuffer>> commandBuffers);
		void submit(const Trident::Queue* queue, view<ref<Trident::CommandBuffer>> commandBuffers, ref<Semaphore> waitSemaphore, ref<Semaphore> signalSemaphore, ref<Trident::Fence> fence);

		void waitIdle(const Trident::Queue* queue);
	}
}