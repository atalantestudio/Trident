#pragma once

#include "Engine/API/CommandEncoder.hpp"
#include "Engine/API/Synchronization/Fence.hpp"
#include "Engine/API/Synchronization/Semaphore.hpp"
#include "Engine/Renderer/Status.hpp"
#include "Engine/Renderer/Swapchain.hpp"

namespace ProjectA {
	class Queue {
		public:
			virtual ~Queue() = default;

			virtual ref<CommandEncoder> createCommandEncoder(const CommandBufferDescriptor& descriptor) const = 0;

			virtual void submit(const ref<CommandBuffer>& commandBuffer, const ref<Semaphore>& waitSemaphore, const ref<Semaphore>& signalSemaphore, const ref<Fence>& fence) const = 0;

			virtual Status present(const chain<ref<Swapchain>>& swapchains, const ref<Semaphore>& waitSemaphore, const chain<uint32>& imageIndices) const = 0;
	};
}