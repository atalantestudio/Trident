// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/CommandEncoder.hpp"
#include "Engine/API/Rendering/Fence.hpp"
#include "Engine/API/Rendering/Semaphore.hpp"
#include "Engine/API/Rendering/Status.hpp"
#include "Engine/API/Rendering/Swapchain.hpp"

namespace Trident {
	class Queue {
		public:
			virtual ~Queue() = default;

			virtual ref<CommandEncoder> createCommandEncoder(const CommandBufferDescriptor& descriptor) const = 0;

			// Executes the provided command buffers.
			// - `waitSemaphore` must be waited upon before submitting.
			// - `signalSemaphore` will be signaled once the command buffers have finished executing.
			// - `fence` will be signaled once the command buffers have finished executing.
			virtual void submit(const sequence<ref<CommandBuffer>>& commandBuffers, ref<Semaphore> waitSemaphore, ref<Semaphore> signalSemaphore, ref<Fence> fence) const = 0;

			// Presents the provided swapchains.
			// - `waitSemaphore ` must be waited upon before presenting.
			virtual PresentStatus present(const sequence<ref<Swapchain>>& swapchains, const sequence<uint32>& imageIndices, ref<Semaphore> waitSemaphore) const = 0;

			// Waits until the queue reaches an idle state.
			virtual void waitIdle() const = 0;
	};
}