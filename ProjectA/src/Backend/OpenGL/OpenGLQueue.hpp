#pragma once

#include "Engine/Renderer/Queue.hpp"

namespace ProjectA {
	class OpenGLDevice;

	class OpenGLQueue : public Queue {
		public:
			OpenGLQueue(const OpenGLDevice& device);

			ref<CommandEncoder> createCommandEncoder(const CommandBufferDescriptor& descriptor) const override;

			void submit(const ref<CommandBuffer>& commandBuffer, const ref<Semaphore>& waitSemaphore, const ref<Semaphore>& signalSemaphore, const ref<Fence>& fence) const override;

		private:
			const OpenGLDevice& device;
	};
}