#include "pch.hpp"

#include "Platform/OpenGL/OpenGLRendererAPI.hpp"

#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/RendererAPI.hpp"

namespace ProjectA {
	scope<RendererAPI> RendererAPI::create() {
		switch (Renderer::getGraphicsApi()) {
			case GraphicsAPI::NONE:
				ENGINE_ASSERT(false, "NONE is not supported yet.");

				return nullptr;
			case GraphicsAPI::OPENGL:
				return createScope<OpenGLRendererAPI>();
			case GraphicsAPI::VULKAN:
				ENGINE_ASSERT(false, "VULKAN is not supported yet.");

				return nullptr;
		}

		return nullptr;
	}
}