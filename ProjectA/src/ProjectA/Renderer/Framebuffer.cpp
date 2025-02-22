#include "pch.hpp"

#include "Platform/OpenGL/OpenGLFramebuffer.hpp"

#include "ProjectA/Renderer/Framebuffer.hpp"
#include "ProjectA/Renderer/Renderer.hpp"

namespace ProjectA {
	ref<Framebuffer> Framebuffer::create(const FramebufferDescriptor& descriptor) {
		switch (Renderer::getGraphicsAPI()) {
			case RendererAPI::GraphicsAPI::None:
				ENGINE_ASSERT(false, "Graphics::None is not supported yet.")

				return nullptr;

			case RendererAPI::GraphicsAPI::OpenGL:
				return createRef<OpenGLFramebuffer>(descriptor);
		}

		return nullptr;
	}

	Framebuffer::Framebuffer(const FramebufferDescriptor& descriptor) : descriptor(descriptor) {}
}