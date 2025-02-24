#include "pch.hpp"

#include "Platform/OpenGL/OpenGLContext.hpp"

#include "Engine/Renderer/Context.hpp"
#include "Engine/Renderer/Renderer.hpp"

namespace ProjectA {
	scope<Context> Context::create(void* window) {
		switch (Renderer::getGraphicsAPI()) {
			case RendererAPI::GraphicsAPI::None:
				ENGINE_ASSERT(false, "Graphics::None is not supported yet.");

				return nullptr;

			case RendererAPI::GraphicsAPI::OpenGL:
				return createScope<OpenGLContext>(static_cast<GLFWwindow*>(window));

			case RendererAPI::GraphicsAPI::Vulkan:
				ENGINE_ASSERT(false, "Graphics::Vulkan is not supported yet.");

				return nullptr;
		}

		return nullptr;
	}
}