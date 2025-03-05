#include "pch.hpp"

#include "Platform/OpenGL/OpenGLCommandBuffer.hpp"
#include "Platform/OpenGL/OpenGLDevice.hpp"

namespace ProjectA {
	OpenGLDevice::OpenGLDevice() {
		PROFILE_SCOPE(FUNCSIG);

		ENGINE_TRACE("[OpenGLDevice]");
	}

	void OpenGLDevice::waitForFence(const ref<Fence>& fence) const {}

	void OpenGLDevice::resetFence(const ref<Fence>& fence) const {}

	uint32 OpenGLDevice::acquireNextImage(const ref<Swapchain>& swapchain, const ref<Semaphore>& semaphore) const {
		return 0;
	}

	void OpenGLDevice::submit(const ref<CommandBuffer>& commandBuffer, const ref<Semaphore>& waitSemaphore, const ref<Semaphore>& signalSemaphore, const ref<Fence>& fence) const {
		const OpenGLCommandBuffer& buffer = *static_cast<const OpenGLCommandBuffer*>(commandBuffer.get());

		for (const ref<OpenGLCommand>& command : buffer) {
			command->execute();
		}
	}

	void OpenGLDevice::present(const ref<Swapchain>& swapchain, const ref<Semaphore>& waitSemaphore, const uint32 imageIndex) const {}

	void OpenGLDevice::waitIdle() const {}
}