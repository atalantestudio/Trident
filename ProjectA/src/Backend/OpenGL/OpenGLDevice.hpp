#pragma once

#include "Backend/OpenGL/OpenGLPhysicalDevice.hpp"
#include "Backend/OpenGL/OpenGLQueue.hpp"
#include "Backend/OpenGL/OpenGLRenderingContext.hpp"

#include "Engine/Renderer/Device.hpp"

namespace ProjectA {
	class OpenGLDevice : public Device {
		public:
			OpenGLDevice(const OpenGLRenderingContext& renderingContext);

			~OpenGLDevice() final override;

			ref<PhysicalDevice> getPhysicalDevice() const final override;

			ref<Queue> getComputeQueue() const final override;

			ref<Queue> getGraphicsQueue() const final override;

			ref<Queue> getPresentQueue() const final override;

			void waitForFence(const ref<Fence>& fence) const final override;

			void resetFence(const ref<Fence>& fence) const final override;

			AcquireNextImageStatus acquireNextImage(const ref<Swapchain>& swapchain, const ref<Semaphore>& semaphore, uint32& imageIndex) const final override;

			void waitIdle() const final override;

		private:
			const ref<OpenGLPhysicalDevice> physicalDevice;
			const ref<OpenGLQueue> queue;

			GLuint vertexArray;
	};
}