#include "pch.hpp"

#include "Backend/OpenGL/OpenGLCommandBuffer.hpp"
#include "Backend/OpenGL/OpenGLDevice.hpp"
#include "Backend/OpenGL/OpenGLFence.hpp"

namespace ProjectA {
	OpenGLDevice::OpenGLDevice(const OpenGLInstance& instance) :
		physicalDevice(createRef<OpenGLPhysicalDevice>()),
		queue(createRef<OpenGLQueue>(instance, *this))
	{}

	ref<PhysicalDevice> OpenGLDevice::getPhysicalDevice() const {
		return physicalDevice;
	}

	ref<Queue> OpenGLDevice::getQueue(QueueFamilyCapability queueFamilyCapability) const {
		return queue;
	}

	void OpenGLDevice::waitForFence(const ref<Fence>& fence) const {
		#if OPENGL_VERSION >= OPENGL_VERSION_3_2
			const ref<OpenGLFence>& glFence = std::static_pointer_cast<OpenGLFence>(fence);

			glFence->create();

			const GLenum status = glClientWaitSync(glFence->object, GL_SYNC_FLUSH_COMMANDS_BIT, max<uint64>());

			DEBUG_ASSERT(status == GL_CONDITION_SATISFIED || status == GL_ALREADY_SIGNALED);
		#else
			ENGINE_ERROR_UNSUPPORTED();
		#endif
	}

	void OpenGLDevice::resetFence(const ref<Fence>& fence) const {
		const ref<OpenGLFence>& glFence = std::static_pointer_cast<OpenGLFence>(fence);

		glFence->reset();
	}

	AcquireNextImageStatus OpenGLDevice::acquireNextImage(const ref<Swapchain>& swapchain, const ref<Semaphore>& semaphore, uint32& imageIndex) const {
		// NOTE
		// WGL handles double buffering at a lower level.
		// The device implementation doesn't need to emulate this behavior and can always return 0.
		imageIndex = 0;

		return AcquireNextImageStatus::SUCCESS;
	}

	void OpenGLDevice::waitIdle() const {}
}