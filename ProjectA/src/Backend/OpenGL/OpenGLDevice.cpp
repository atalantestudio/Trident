#include "pch.hpp"

#include "Backend/OpenGL/OpenGLCommandBuffer.hpp"
#include "Backend/OpenGL/OpenGLDevice.hpp"

namespace ProjectA {
	OpenGLDevice::OpenGLDevice() {
		PROFILE_SCOPE(FUNCSIG);

		ENGINE_TRACE("[OpenGLDevice]");
	}

	void OpenGLDevice::waitForFence(const ref<Fence>& fence) const {}

	void OpenGLDevice::resetFence(const ref<Fence>& fence) const {}

	Status OpenGLDevice::acquireNextImage(const ref<Swapchain>& swapchain, const ref<Semaphore>& semaphore, uint32& nextImageIndex) const {
		nextImageIndex = 0;

		return Status::SUCCESS;
	}

	void OpenGLDevice::submit(const ref<CommandBuffer>& commandBuffer, const ref<Semaphore>& waitSemaphore, const ref<Semaphore>& signalSemaphore, const ref<Fence>& fence) const {
		const OpenGLCommandBuffer& buffer = *static_cast<const OpenGLCommandBuffer*>(commandBuffer.get());

		for (const ref<OpenGLCommand>& command : buffer) {
			command->execute();
		}
	}

	Status OpenGLDevice::present(const ref<Swapchain>& swapchain, const ref<Semaphore>& waitSemaphore, uint32 imageIndex) const {
		return Status::SUCCESS;
	}

	void OpenGLDevice::waitIdle() const {}
}