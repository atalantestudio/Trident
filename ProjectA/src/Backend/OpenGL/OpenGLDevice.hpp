#pragma once

#include "Backend/OpenGL/OpenGLQueue.hpp"

#include "Engine/Renderer/Device.hpp"

namespace ProjectA {
	class OpenGLDevice : public Device {
		public:
			OpenGLDevice();

			ref<Queue> getComputeQueue() const override;

			ref<Queue> getGraphicsQueue() const override;

			void waitForFence(const ref<Fence>& fence) const override;

			void resetFence(const ref<Fence>& fence) const override;

			Status acquireNextImage(const ref<Swapchain>& swapchain, const ref<Semaphore>& semaphore, uint32& nextImageIndex) const override;

			Status present(const ref<Swapchain>& swapchain, const ref<Semaphore>& waitSemaphore, uint32 imageIndex) const override;

			void waitIdle() const override;

		private:
			const ref<OpenGLQueue> queue;
	};
}