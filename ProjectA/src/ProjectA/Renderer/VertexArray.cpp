#include "pch.hpp"

#include "Platform/OpenGL/OpenGLVertexArray.hpp"

#include "ProjectA/Renderer/Renderer.hpp"
#include "ProjectA/Renderer/VertexArray.hpp"

namespace ProjectA {
	ref<VertexArray> VertexArray::create() {
		switch (Renderer::getGraphicsAPI()) {
			case RendererAPI::GraphicsAPI::None:
				ENGINE_ASSERT(false, "Graphics::None is not supported yet.");

				return nullptr;

			case RendererAPI::GraphicsAPI::OpenGL:
				return createRef<OpenGLVertexArray>();

			case RendererAPI::GraphicsAPI::Vulkan:
				ENGINE_ASSERT(false, "Graphics::Vulkan is not supported yet.");

				return nullptr;
		}

		return nullptr;
	}
}