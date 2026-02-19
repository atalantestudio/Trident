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

			/// Creates and returns a command encoder with the capabilities of the queue.
			// TODO
			virtual ref<CommandEncoder> createCommandEncoder(const CommandBufferDescriptor& descriptor) const = 0;
			// ref<CommandEncoder> createCommandEncoder(const CommandBufferDescriptor& descriptor) const;

			/// Executes the provided command buffers.
			virtual void submit(view<ref<CommandBuffer>> commandBuffers) const = 0;

			/// Executes the provided command buffers.
			virtual void submit(
				/// The command buffers to execute.
				view<ref<CommandBuffer>> commandBuffers,

				/// (Optional)
				/// Must be waited on before submitting the command buffers.
				ref<Semaphore> waitSemaphore,

				/// (Optional)
				/// Will be signaled once the command buffers have finished executing.
				ref<Semaphore> signalSemaphore,

				/// Will be signaled once the command buffers have finished executing.
				ref<Fence> fence
			) const = 0;

			/// Presents the provided swapchains.
			virtual PresentStatus present(
				/// The swapchains to present.
				view<ref<Swapchain>> swapchains,

				/// The indices of the swapchain presentable images.
				/// There is 1 presentable image per swapchain, so this view must have the same number of elements as `swapchains`.
				view<uint32> presentableImageIndices,

				/// Must be waited on before presenting the swapchains.
				ref<Semaphore> waitSemaphore
			) const = 0;

			/// Waits until the queue reaches an idle state.
			virtual void waitIdle() const = 0;

		private:
			// TODO
			// const GraphicsAdapter* adapter;
	};
}