#include "pch.hpp"

#include <glad/glad.h>

#include "Backend/OpenGL/OpenGLCommandBuffer.hpp"
#include "Backend/OpenGL/OpenGLDevice.hpp"
#include "Backend/OpenGL/OpenGLFence.hpp"

namespace ProjectA {
	OpenGLDevice::OpenGLDevice() :
		physicalDevice(createRef<OpenGLPhysicalDevice>()),
		queue(createRef<OpenGLQueue>(*this))
	{}

	ref<PhysicalDevice> OpenGLDevice::getPhysicalDevice() const {
		return physicalDevice;
	}

	ref<Queue> OpenGLDevice::getComputeQueue() const {
		return queue;
	}

	ref<Queue> OpenGLDevice::getGraphicsQueue() const {
		return queue;
	}

	void OpenGLDevice::waitForFence(const ref<Fence>& fence) const {
		OpenGLFence& glFence = *std::static_pointer_cast<OpenGLFence>(fence);

		glFence.create();

		// TODO: Use glClientWaitSync or glWaitSync?
		// UINT64_MAX disables the timeout.
		const GLenum status = glClientWaitSync(glFence.object, GL_SYNC_FLUSH_COMMANDS_BIT, UINT64_MAX);

		// TODO: Fix crash here.
		ENGINE_ASSERT(status == GL_CONDITION_SATISFIED, "Failed waiting for OpenGL fence (code: []).", status);
	}

	void OpenGLDevice::resetFence(const ref<Fence>& fence) const {
		OpenGLFence& glFence = *std::static_pointer_cast<OpenGLFence>(fence);

		glFence.reset();
	}

	Status OpenGLDevice::acquireNextImage(const ref<Swapchain>& swapchain, const ref<Semaphore>& semaphore, uint32& nextImageIndex) const {
		// TODO: Return correct image index - suitable for multi-buffering.
		nextImageIndex = 0;

		return Status::SUCCESS;
	}

	Status OpenGLDevice::present(const ref<Swapchain>& swapchain, const ref<Semaphore>& waitSemaphore, uint32 imageIndex) const {
		return Status::SUCCESS;
	}

	void OpenGLDevice::waitIdle() const {}
}