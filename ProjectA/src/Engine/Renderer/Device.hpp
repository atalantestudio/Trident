#pragma once

#include "Engine/API/Synchronization/Fence.hpp"
#include "Engine/API/Synchronization/Semaphore.hpp"
#include "Engine/Renderer/CommandBuffer.hpp"
#include "Engine/Renderer/Status.hpp"
#include "Engine/Renderer/Swapchain.hpp"

namespace ProjectA {
	class Device {
		public:
			virtual ~Device() = default;

			virtual void waitForFence(const ref<Fence>& fence) const = 0;

			virtual void resetFence(const ref<Fence>& fence) const = 0;

			// Writes the index of the next available swapchain image.
			virtual Status acquireNextImage(const ref<Swapchain>& swapchain, const ref<Semaphore>& semaphore, uint32& nextImageIndex) const = 0;

			virtual void submit(const ref<CommandBuffer>& commandBuffer, const ref<Semaphore>& waitSemaphore, const ref<Semaphore>& signalSemaphore, const ref<Fence>& fence) const = 0;

			virtual Status present(const ref<Swapchain>& swapchain, const ref<Semaphore>& waitSemaphore, uint32 imageIndex) const = 0;

			virtual void waitIdle() const = 0;
	};
}