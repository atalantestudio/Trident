#include "pch.hpp"
#include "Platform/OpenGL/OpenGLVertexBuffer.hpp"
#include "ProjectA/Renderer/Renderer.hpp"
#include "ProjectA/Renderer/VertexBuffer.hpp"

namespace ProjectA {
	ref<VertexBuffer> VertexBuffer::create(float* vertices, uint32_t size) {
		switch (Renderer::getGraphicsAPI()) {
			case RendererAPI::GraphicsAPI::None:
				ENGINE_ASSERT(false, "Graphics::None is not supported yet.")

				return nullptr;

			case RendererAPI::GraphicsAPI::OpenGL:
				return createRef<OpenGLVertexBuffer>(vertices, size);
		}

		return nullptr;
	}
}