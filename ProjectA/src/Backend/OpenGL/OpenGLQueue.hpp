#pragma once

#include "Backend/OpenGL/OpenGLInstance.hpp"

#include "Engine/API/Rendering/Queue.hpp"

namespace ProjectA {
	class OpenGLDevice;

	class OpenGLQueue final : public Queue {
		public:
			explicit OpenGLQueue(const OpenGLInstance& instance, const OpenGLDevice& device);

			ref<CommandEncoder> createCommandEncoder(const CommandBufferDescriptor& descriptor) const final override;

			void submit(const sequence<ref<CommandBuffer>>& commandBuffers, const ref<Semaphore>& waitSemaphore, const ref<Semaphore>& signalSemaphore, const ref<Fence>& fence) const final override;

			PresentStatus present(const sequence<ref<Swapchain>>& swapchains, const ref<Semaphore>& waitSemaphore, const sequence<uint32>& imageIndices) const final override;

		private:
			const OpenGLInstance* instance;
			const OpenGLDevice* device;
	};
}