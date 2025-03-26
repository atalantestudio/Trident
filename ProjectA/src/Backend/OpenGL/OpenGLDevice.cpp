#include "pch.hpp"

#include <glad/glad.h>

#include "Backend/OpenGL/OpenGLCommandBuffer.hpp"
#include "Backend/OpenGL/OpenGLDevice.hpp"
#include "Backend/OpenGL/OpenGLFence.hpp"

namespace ProjectA {
	OpenGLDevice::OpenGLDevice() {
		PROFILE_SCOPE(FUNCSIG);

		ENGINE_TRACE("[OpenGLDevice]");
	}

	void OpenGLDevice::waitForFence(const ref<Fence>& fence) const {
		OpenGLFence* glFence = static_cast<OpenGLFence*>(fence.get());

		glFence->create();

		// TODO: Use glClientWaitSync or glWaitSync?
		const GLenum status = glClientWaitSync(glFence->object, GL_SYNC_FLUSH_COMMANDS_BIT, UINT64_MAX);

		ENGINE_ASSERT(status == GL_CONDITION_SATISFIED, "Failed waiting for OpenGL fence (code: []).", status);
	}

	void OpenGLDevice::resetFence(const ref<Fence>& fence) const {
		OpenGLFence* glFence = static_cast<OpenGLFence*>(fence.get());

		glFence->reset();
	}

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