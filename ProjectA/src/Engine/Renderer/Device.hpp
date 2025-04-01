#pragma once

#include "Engine/Renderer/Queue.hpp"
#include "Engine/Renderer/Status.hpp"
#include "Engine/Renderer/Swapchain.hpp"

namespace ProjectA {
	class Device {
		public:
			virtual ~Device() = default;

			virtual ref<Queue> getComputeQueue() const = 0;

			virtual ref<Queue> getGraphicsQueue() const = 0;

			virtual void waitForFence(const ref<Fence>& fence) const = 0;

			virtual void resetFence(const ref<Fence>& fence) const = 0;

			// Writes the index of the next available swapchain image.
			virtual Status acquireNextImage(const ref<Swapchain>& swapchain, const ref<Semaphore>& semaphore, uint32& nextImageIndex) const = 0;

			virtual Status present(const ref<Swapchain>& swapchain, const ref<Semaphore>& waitSemaphore, uint32 imageIndex) const = 0;

			virtual void waitIdle() const = 0;
	};
}