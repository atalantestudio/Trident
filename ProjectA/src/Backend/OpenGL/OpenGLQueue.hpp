#pragma once

#include "Backend/OpenGL/OpenGLRenderingContext.hpp"

#include "Engine/Renderer/Queue.hpp"

namespace ProjectA {
	class OpenGLDevice;

	class OpenGLQueue : public Queue {
		public:
			OpenGLQueue(const OpenGLRenderingContext& renderingContext, const OpenGLDevice& device);

			ref<CommandEncoder> createCommandEncoder(const CommandBufferDescriptor& descriptor) const final override;

			void submit(const ref<CommandBuffer>& commandBuffer, const ref<Semaphore>& waitSemaphore, const ref<Semaphore>& signalSemaphore, const ref<Fence>& fence) const final override;

			PresentStatus present(const std::vector<ref<Swapchain>>& swapchains, const ref<Semaphore>& waitSemaphore, const std::vector<uint32>& imageIndices) const final override;

		private:
			const OpenGLRenderingContext& renderingContext;
			const OpenGLDevice& device;
	};
}