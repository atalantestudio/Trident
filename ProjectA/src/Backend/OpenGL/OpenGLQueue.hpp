#pragma once

#include "Engine/API/DeviceContext.hpp"
#include "Engine/Renderer/Queue.hpp"

namespace ProjectA {
	class OpenGLDevice;

	class OpenGLQueue : public Queue {
		public:
			OpenGLQueue(const DeviceContext& deviceContext, const OpenGLDevice& device);

			ref<CommandEncoder> createCommandEncoder(const CommandBufferDescriptor& descriptor) const final override;

			void submit(const ref<CommandBuffer>& commandBuffer, const ref<Semaphore>& waitSemaphore, const ref<Semaphore>& signalSemaphore, const ref<Fence>& fence) const final override;

			Status present(const chain<ref<Swapchain>>& swapchains, const ref<Semaphore>& waitSemaphore, const chain<uint32>& imageIndices) const final override;

		private:
			const DeviceContext& deviceContext;
			const OpenGLDevice& device;
	};
}