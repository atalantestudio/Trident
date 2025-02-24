#include "pch.hpp"

#include "Platform/OpenGL/OpenGLContext.hpp"

#include "Engine/Renderer/Context.hpp"
#include "Engine/Renderer/Renderer.hpp"

namespace ProjectA {
	scope<Context> Context::create(void* window) {
		switch (Renderer::getGraphicsApi()) {
			case GraphicsAPI::NONE:
				ENGINE_ASSERT(false, "Graphics::NONE is not supported yet.");

				return nullptr;

			case GraphicsAPI::OPENGL:
				return createScope<OpenGLContext>(static_cast<GLFWwindow*>(window));

			case GraphicsAPI::VULKAN:
				ENGINE_ASSERT(false, "Graphics::VULKAN is not supported yet.");

				return nullptr;
		}

		return nullptr;
	}
}