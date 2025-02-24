#include "pch.hpp"

#include "Platform/OpenGL/OpenGLFramebuffer.hpp"

#include "Engine/Renderer/Framebuffer.hpp"
#include "Engine/Renderer/Renderer.hpp"

namespace ProjectA {
	ref<Framebuffer> Framebuffer::create(const FramebufferDescriptor& descriptor) {
		switch (Renderer::getGraphicsApi()) {
			case GraphicsAPI::NONE:
				ENGINE_ASSERT(false, "Graphics::NONE is not supported yet.");

				return nullptr;
			case GraphicsAPI::OPENGL:
				return createRef<OpenGLFramebuffer>(descriptor);
			case GraphicsAPI::VULKAN:
				ENGINE_ASSERT(false, "Graphics::VULKAN is not supported yet.");

				return nullptr;
		}

		return nullptr;
	}

	Framebuffer::Framebuffer(const FramebufferDescriptor& descriptor) : descriptor(descriptor) {}
}