#include "pch.hpp"

#include "Platform/OpenGL/OpenGLContext.hpp"

#include "ProjectA/Renderer/Context.hpp"
#include "ProjectA/Renderer/Renderer.hpp"

namespace ProjectA {
	Scope<Context> Context::create(void* window) {
		switch (Renderer::getGraphicsAPI()) {
			case RendererAPI::GraphicsAPI::None:
				ENGINE_ASSERT(false, "Graphics::None is not supported yet.")

				return nullptr;

			case RendererAPI::GraphicsAPI::OpenGL:
				return CreateScope<OpenGLContext>(static_cast<GLFWwindow*>(window));
		}

		return nullptr;
	}
}