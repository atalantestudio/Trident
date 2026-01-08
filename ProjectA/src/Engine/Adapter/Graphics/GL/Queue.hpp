#pragma once

#include "Engine/Adapter/Graphics/GL/Instance.hpp"

#include "Engine/API/Rendering/Queue.hpp"

namespace ProjectA {
	class OpenGLDevice;

	class OpenGLQueue final : public Queue {
		public:
			explicit OpenGLQueue(const OpenGLInstance& instance, const OpenGLDevice& device);

			ref<CommandEncoder> createCommandEncoder(const CommandBufferDescriptor& descriptor) const final override;

			void submit(const sequence<ref<CommandBuffer>>& commandBuffers, ref<Semaphore> waitSemaphore, ref<Semaphore> signalSemaphore, ref<Fence> fence) const final override;

			PresentStatus present(const sequence<ref<Swapchain>>& swapchains, ref<Semaphore> waitSemaphore, const sequence<uint32>& imageIndices) const final override;

			void waitIdle() const final override;

		private:
			const OpenGLInstance* instance;
			const OpenGLDevice* device;
	};
}