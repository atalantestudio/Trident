#include "pch.hpp"

#include "Backend/OpenGL/OpenGLCommandBuffer.hpp"
#include "Backend/OpenGL/OpenGLCommandEncoder.hpp"
#include "Backend/OpenGL/OpenGLDevice.hpp"
#include "Backend/OpenGL/OpenGLQueue.hpp"

namespace ProjectA {
	OpenGLQueue::OpenGLQueue(const OpenGLInstance& instance, const OpenGLDevice& device) :
		instance(&instance),
		device(&device)
	{}

	ref<CommandEncoder> OpenGLQueue::createCommandEncoder(const CommandBufferDescriptor& descriptor) const {
		DEBUG_DESCRIPTOR_GUARD(descriptor);

		return createRef<OpenGLCommandEncoder>(*device, descriptor);
	}

	void OpenGLQueue::submit(const sequence<ref<CommandBuffer>>& commandBuffers, const ref<Semaphore>& waitSemaphore, const ref<Semaphore>& signalSemaphore, const ref<Fence>& fence) const {
		for (const ref<CommandBuffer>& commandBuffer : commandBuffers) {
			const OpenGLCommandBuffer& glCommandBuffer = *std::static_pointer_cast<OpenGLCommandBuffer>(commandBuffer);

			for (const ref<OpenGLCommand>& command : glCommandBuffer) {
				command->execute();
			}
		}

		if (fence != nullptr) {
			device->waitForFence(fence);
		}
	}

	PresentStatus OpenGLQueue::present(const sequence<ref<Swapchain>>& swapchains, const ref<Semaphore>& waitSemaphore, const sequence<uint32>& imageIndices) const {
		instance->swapBuffers();

		return PresentStatus::SUCCESS;
	}
}