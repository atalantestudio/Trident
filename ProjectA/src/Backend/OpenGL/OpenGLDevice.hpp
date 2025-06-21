#pragma once

#include "Backend/OpenGL/OpenGLPhysicalDevice.hpp"
#include "Backend/OpenGL/OpenGLQueue.hpp"
#include "Backend/OpenGL/OpenGLRenderingContext.hpp"

#include "Engine/API/Rendering/Device.hpp"

namespace ProjectA {
	class OpenGLDevice final : public Device {
		public:
			explicit OpenGLDevice(const OpenGLRenderingContext& renderingContext);

			ref<PhysicalDevice> getPhysicalDevice() const final override;

			ref<Queue> getQueue(QueueFamilyCapability queueFamilyCapability) const final override;

			void waitForFence(const ref<Fence>& fence) const final override;

			void resetFence(const ref<Fence>& fence) const final override;

			AcquireNextImageStatus acquireNextImage(const ref<Swapchain>& swapchain, const ref<Semaphore>& semaphore, uint32& imageIndex) const final override;

			void waitIdle() const final override;

		private:
			const ref<OpenGLPhysicalDevice> physicalDevice;
			const ref<OpenGLQueue> queue;
	};
}