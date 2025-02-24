#include "pch.hpp"

#include "Platform/OpenGL/OpenGLFramebuffer.hpp"

#include "Engine/Renderer/Framebuffer.hpp"
#include "Engine/Renderer/Renderer.hpp"

namespace ProjectA {
	ref<Framebuffer> Framebuffer::create(const FramebufferDescriptor& descriptor) {
		switch (Renderer::getGraphicsAPI()) {
			case RendererAPI::GraphicsAPI::None:
				ENGINE_ASSERT(false, "Graphics::None is not supported yet.");

				return nullptr;

			case RendererAPI::GraphicsAPI::OpenGL:
				return createRef<OpenGLFramebuffer>(descriptor);

			case RendererAPI::GraphicsAPI::Vulkan:
				ENGINE_ASSERT(false, "Graphics::Vulkan is not supported yet.");

				return nullptr;
		}

		return nullptr;
	}

	Framebuffer::Framebuffer(const FramebufferDescriptor& descriptor) : descriptor(descriptor) {}
}