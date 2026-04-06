// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/GL/Device.hpp"
#include "Engine/Adapter/Graphics/GL/Instance.hpp"
#include "Engine/API/Graphics/Queue.hpp"

namespace Trident {
	class GL::Queue final : public Trident::Queue {
		public:
			explicit Queue(const Trident::Device* device, uint32 queueFamilyIndex, QueueFamilyCapability queueFamilyCapability);
	};

	namespace GL {
		ref<Trident::Queue> createQueue(const Trident::Device* device, uint32 queueFamilyIndex, QueueFamilyCapability queueFamilyCapability);

		void destroyQueue(const Trident::Queue* queue);

		PresentStatus present(const Trident::Queue* queue, view<ref<Trident::Swapchain>> swapchains, view<uint32> presentableImageIndices, ref<Trident::Semaphore> waitSemaphore);

		void submit(const Trident::Queue* queue, view<ref<Trident::CommandBuffer>> commandBuffers);
		void submit(const Trident::Queue* queue, view<ref<Trident::CommandBuffer>> commandBuffers, ref<Trident::Semaphore> waitSemaphore, ref<Trident::Semaphore> signalSemaphore, ref<Trident::Fence> fence);

		void waitIdle(const Trident::Queue* queue);
	}
}