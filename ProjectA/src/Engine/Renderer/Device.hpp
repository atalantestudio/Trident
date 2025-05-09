#pragma once

#include "Engine/Renderer/PhysicalDevice.hpp"
#include "Engine/Renderer/Queue.hpp"
#include "Engine/Renderer/Status.hpp"

namespace ProjectA {
	class Device {
		public:
			virtual ~Device() = default;

			virtual ref<PhysicalDevice> getPhysicalDevice() const = 0;

			virtual ref<Queue> getComputeQueue() const = 0;

			virtual ref<Queue> getGraphicsQueue() const = 0;

			virtual ref<Queue> getPresentQueue() const = 0;

			virtual void waitForFence(const ref<Fence>& fence) const = 0;

			virtual void resetFence(const ref<Fence>& fence) const = 0;

			// Writes the index of the next available swapchain image.
			virtual Status acquireNextImage(const ref<Swapchain>& swapchain, const ref<Semaphore>& semaphore, uint32& nextImageIndex) const = 0;

			virtual void waitIdle() const = 0;
	};
}