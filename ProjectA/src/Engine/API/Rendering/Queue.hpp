#pragma once

#include "Engine/API/Rendering/CommandEncoder.hpp"
#include "Engine/API/Rendering/Fence.hpp"
#include "Engine/API/Rendering/Semaphore.hpp"
#include "Engine/API/Rendering/Status.hpp"
#include "Engine/API/Rendering/Swapchain.hpp"

namespace ProjectA {
	class Queue {
		public:
			virtual ~Queue() = default;

			virtual ref<CommandEncoder> createCommandEncoder(const CommandBufferDescriptor& descriptor) const = 0;

			virtual void submit(const sequence<ref<CommandBuffer>>& commandBuffers, const ref<Semaphore>& waitSemaphore, const ref<Semaphore>& signalSemaphore, const ref<Fence>& fence) const = 0;

			virtual PresentStatus present(const sequence<ref<Swapchain>>& swapchains, const ref<Semaphore>& waitSemaphore, const sequence<uint32>& imageIndices) const = 0;

			// Waits until the queue reaches an idle state.
			virtual void waitIdle() const = 0;
	};
}