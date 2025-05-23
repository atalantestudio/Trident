#include "pch.hpp"

#include "Backend/OpenGL/OpenGLCommandBuffer.hpp"
#include "Backend/OpenGL/OpenGLDevice.hpp"
#include "Backend/OpenGL/OpenGLFence.hpp"

namespace ProjectA {
	OpenGLDevice::OpenGLDevice(const OpenGLRenderingContext& renderingContext) :
		physicalDevice(createRef<OpenGLPhysicalDevice>()),
		queue(createRef<OpenGLQueue>(renderingContext, *this))
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

	ref<Queue> OpenGLDevice::getPresentQueue() const {
		return queue;
	}

	void OpenGLDevice::waitForFence(const ref<Fence>& fence) const {
		OpenGLFence& glFence = *std::static_pointer_cast<OpenGLFence>(fence);

		glFence.create();

		// UINT64_MAX disables the timeout.
		const GLenum status = glClientWaitSync(glFence.object, GL_SYNC_FLUSH_COMMANDS_BIT, UINT64_MAX);

		// TODO: Fix crash here.
		ENGINE_ASSERT(status == GL_CONDITION_SATISFIED, "Failed waiting for OpenGL fence (code: []).", status);
	}

	void OpenGLDevice::resetFence(const ref<Fence>& fence) const {
		OpenGLFence& glFence = *std::static_pointer_cast<OpenGLFence>(fence);

		glFence.reset();
	}

	Status OpenGLDevice::acquireNextImage(const ref<Swapchain>& swapchain, const ref<Semaphore>& semaphore, uint32& imageIndex) const {
		// TODO: Does imageIndex need to be correct
		// since the double buffering is handled by the device context anyways?
		imageIndex = 0;

		return Status::SUCCESS;
	}

	void OpenGLDevice::waitIdle() const {}
}