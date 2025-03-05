#pragma once

#include "Engine/Renderer/CommandBuffer.hpp"
#include "Engine/Renderer/Swapchain.hpp"
#include "Engine/Synchronization/Fence.hpp"
#include "Engine/Synchronization/Semaphore.hpp"

namespace ProjectA {
	class Device {
		public:
			virtual ~Device() = default;

			virtual void waitForFence(const ref<Fence>& fence) const = 0;

			virtual void resetFence(const ref<Fence>& fence) const = 0;

			// Returns the index of the next available swapchain image.
			virtual uint32 acquireNextImage(const ref<Swapchain>& swapchain, const ref<Semaphore>& semaphore) const = 0;

			virtual void submit(const ref<CommandBuffer>& commandBuffer, const ref<Semaphore>& waitSemaphore, const ref<Semaphore>& signalSemaphore, const ref<Fence>& fence) const = 0;

			virtual void present(const ref<Swapchain>& swapchain, const ref<Semaphore>& waitSemaphore, const uint32 imageIndex) const = 0;

			virtual void waitIdle() const = 0;
	};
}