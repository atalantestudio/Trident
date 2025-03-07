#pragma once

#include "Engine/Renderer/Device.hpp"

namespace ProjectA {
	class OpenGLDevice : public Device {
		public:
			OpenGLDevice();

			void waitForFence(const ref<Fence>& fence) const override;

			void resetFence(const ref<Fence>& fence) const override;

			uint32 acquireNextImage(const ref<Swapchain>& swapchain, const ref<Semaphore>& semaphore) const override;

			void submit(const ref<CommandBuffer>& commandBuffer, const ref<Semaphore>& waitSemaphore, const ref<Semaphore>& signalSemaphore, const ref<Fence>& fence) const override;

			void present(const ref<Swapchain>& swapchain, const ref<Semaphore>& waitSemaphore, const uint32 imageIndex) const override;

			void waitIdle() const override;
	};
}