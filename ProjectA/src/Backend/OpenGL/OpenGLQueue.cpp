#include "pch.hpp"

#include "Backend/OpenGL/OpenGLCommandBuffer.hpp"
#include "Backend/OpenGL/OpenGLCommandEncoder.hpp"
#include "Backend/OpenGL/OpenGLDevice.hpp"
#include "Backend/OpenGL/OpenGLQueue.hpp"

namespace ProjectA {
	OpenGLQueue::OpenGLQueue(const OpenGLRenderingContext& renderingContext, const OpenGLDevice& device) :
		renderingContext(renderingContext),
		device(device)
	{}

	ref<CommandEncoder> OpenGLQueue::createCommandEncoder(const CommandBufferDescriptor& descriptor) const {
		return createRef<OpenGLCommandEncoder>(device, descriptor);
	}

	void OpenGLQueue::submit(const ref<CommandBuffer>& commandBuffer, const ref<Semaphore>& waitSemaphore, const ref<Semaphore>& signalSemaphore, const ref<Fence>& fence) const {
		const OpenGLCommandBuffer& glCommandBuffer = *std::static_pointer_cast<OpenGLCommandBuffer>(commandBuffer).get();

		for (const ref<OpenGLCommand>& command : glCommandBuffer) {
			command->execute();
		}

		if (fence != nullptr) {
			device.waitForFence(fence);
		}
	}

	PresentStatus OpenGLQueue::present(const std::vector<ref<Swapchain>>& swapchains, const ref<Semaphore>& waitSemaphore, const std::vector<uint32>& imageIndices) const {
		renderingContext.swapBuffers();

		return PresentStatus::SUCCESS;
	}
}