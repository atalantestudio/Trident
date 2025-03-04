#include "pch.hpp"

#include "Platform/OpenGL/OpenGLCommandBuffer.hpp"
#include "Platform/OpenGL/OpenGLDevice.hpp"

namespace ProjectA {
	OpenGLDevice::OpenGLDevice() {
		PROFILE_SCOPE(FUNCSIG);

		ENGINE_TRACE("[OpenGLDevice]");
	}

	void OpenGLDevice::waitForFences() const {}

	void OpenGLDevice::resetFences() const {}

	uint32 OpenGLDevice::acquireNextImage(const ref<Swapchain>& swapchain) const {
		return 0;
	}

	void OpenGLDevice::submit(const ref<CommandBuffer>& commandBuffer) const {
		const OpenGLCommandBuffer& buffer = *static_cast<const OpenGLCommandBuffer*>(commandBuffer.get());

		for (const ref<OpenGLCommand>& command : buffer) {
			command->execute();
		}
	}

	void OpenGLDevice::present(const ref<Swapchain>& swapchain, const uint32 imageIndex) const {}

	void OpenGLDevice::waitIdle() const {}
}