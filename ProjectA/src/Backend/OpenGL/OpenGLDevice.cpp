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

	ref<Queue> OpenGLDevice::getQueue(QueueFamilyCapability queueFamilyCapability) const {
		return queue;
	}

	void OpenGLDevice::waitForFence(const ref<Fence>& fence) const {
		const ref<OpenGLFence>& glFence = std::static_pointer_cast<OpenGLFence>(fence);

		glFence->create();

		const GLenum status = glClientWaitSync(glFence->object, GL_SYNC_FLUSH_COMMANDS_BIT, std::numeric_limits<uint64>::max());

		DEBUG_ASSERT(status == GL_CONDITION_SATISFIED || status == GL_ALREADY_SIGNALED);
	}

	void OpenGLDevice::resetFence(const ref<Fence>& fence) const {
		const ref<OpenGLFence>& glFence = std::static_pointer_cast<OpenGLFence>(fence);

		glFence->reset();
	}

	AcquireNextImageStatus OpenGLDevice::acquireNextImage(const ref<Swapchain>& swapchain, const ref<Semaphore>& semaphore, uint32& imageIndex) const {
		// TODO: Does imageIndex need to be correct since the double buffering is handled by the device context anyways?
		imageIndex = 0;

		return AcquireNextImageStatus::SUCCESS;
	}

	void OpenGLDevice::waitIdle() const {}
}