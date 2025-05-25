#pragma once

#include "Engine/API/Rendering/CommandEncoder.hpp"
#include "Engine/API/Rendering/Swapchain.hpp"
#include "Engine/API/Synchronization/Fence.hpp"
#include "Engine/API/Synchronization/Semaphore.hpp"
#include "Engine/Renderer/Status.hpp"

namespace ProjectA {
	class Queue {
		public:
			virtual ~Queue() = default;

			virtual ref<CommandEncoder> createCommandEncoder(const CommandBufferDescriptor& descriptor) const = 0;

			virtual void submit(const ref<CommandBuffer>& commandBuffer, const ref<Semaphore>& waitSemaphore, const ref<Semaphore>& signalSemaphore, const ref<Fence>& fence) const = 0;

			virtual PresentStatus present(const std::vector<ref<Swapchain>>& swapchains, const ref<Semaphore>& waitSemaphore, const std::vector<uint32>& imageIndices) const = 0;
	};
}