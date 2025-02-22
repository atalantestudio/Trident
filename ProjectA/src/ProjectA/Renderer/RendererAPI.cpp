#include "pch.hpp"

#include "Platform/OpenGL/OpenGLRendererAPI.hpp"

#include "ProjectA/Renderer/RendererAPI.hpp"

namespace ProjectA {
	RendererAPI::GraphicsAPI RendererAPI::graphicsApi = RendererAPI::GraphicsAPI::OpenGL;

	scope<RendererAPI> RendererAPI::create() {
		switch (graphicsApi) {
			case RendererAPI::GraphicsAPI::None:
				ENGINE_ASSERT(false, "Graphics::None is not supported yet.");

				return nullptr;

			case RendererAPI::GraphicsAPI::OpenGL:
				return createScope<OpenGLRendererAPI>();
		}

		return nullptr;
	}
}