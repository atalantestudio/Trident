#include "pch.hpp"

#include "Engine/Renderer/Framebuffer.hpp"

namespace ProjectA {
	// TODO: Remove.
	/*ref<Framebuffer> Framebuffer::create(const FramebufferDescriptor& descriptor) {
		switch (Renderer::getGraphicsApi()) {
			case GraphicsAPI::NONE:
				ENGINE_ASSERT(false, "NONE is not supported yet.");

				return nullptr;
			case GraphicsAPI::OPENGL:
				return createRef<OpenGLFramebuffer>(descriptor);
			case GraphicsAPI::VULKAN:
				ENGINE_ASSERT(false, "VULKAN is not supported yet.");

				return nullptr;
		}

		return nullptr;
	}*/
}