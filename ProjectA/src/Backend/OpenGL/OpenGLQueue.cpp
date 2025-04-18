#include "pch.hpp"

#include "Backend/OpenGL/OpenGLCommandBuffer.hpp"
#include "Backend/OpenGL/OpenGLCommandEncoder.hpp"
#include "Backend/OpenGL/OpenGLDevice.hpp"
#include "Backend/OpenGL/OpenGLQueue.hpp"

namespace ProjectA {
	OpenGLQueue::OpenGLQueue(const OpenGLDevice& device) :
		device(device)
	{}

	ref<CommandEncoder> OpenGLQueue::createCommandEncoder(const CommandBufferDescriptor& descriptor) const {
		return createRef<OpenGLCommandEncoder>(device, descriptor);
	}

	void OpenGLQueue::submit(const ref<CommandBuffer>& commandBuffer, const ref<Semaphore>& waitSemaphore, const ref<Semaphore>& signalSemaphore, const ref<Fence>& fence) const {
		const OpenGLCommandBuffer& buffer = *std::static_pointer_cast<OpenGLCommandBuffer>(commandBuffer).get();

		for (const ref<OpenGLCommand>& command : buffer) {
			command->execute();
		}
	}

	Status OpenGLQueue::present(const ref<Swapchain>& swapchain, const ref<Semaphore>& waitSemaphore, uint32 imageIndex) const {
		return Status::SUCCESS;
	}
}