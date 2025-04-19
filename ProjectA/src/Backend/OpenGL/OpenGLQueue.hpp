#pragma once

#include "Engine/API/DeviceContext.hpp"
#include "Engine/Renderer/Queue.hpp"

namespace ProjectA {
	class OpenGLDevice;

	class OpenGLQueue : public Queue {
		public:
			OpenGLQueue(const DeviceContext& deviceContext, const OpenGLDevice& device);

			ref<CommandEncoder> createCommandEncoder(const CommandBufferDescriptor& descriptor) const override;

			void submit(const ref<CommandBuffer>& commandBuffer, const ref<Semaphore>& waitSemaphore, const ref<Semaphore>& signalSemaphore, const ref<Fence>& fence) const override;

			Status present(const ref<Swapchain>& swapchain, const ref<Semaphore>& waitSemaphore, uint32 imageIndex) const override;

		private:
			const DeviceContext& deviceContext;
			const OpenGLDevice& device;
	};
}