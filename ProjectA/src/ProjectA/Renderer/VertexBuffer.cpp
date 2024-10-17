#include "pch.hpp"
#include "Platform/OpenGL/OpenGLVertexBuffer.hpp"
#include "Renderer.hpp"
#include "VertexBuffer.hpp"

namespace ProjectA {
	Ref<VertexBuffer> VertexBuffer::create(float* vertices, uint32_t size) {
		switch (Renderer::getGraphicsAPI()) {
			case RendererAPI::GraphicsAPI::None:
				CORE_ASSERT(false, "Graphics::None is not supported yet.")

				return nullptr;

			case RendererAPI::GraphicsAPI::OpenGL:
				return std::make_shared<OpenGLVertexBuffer>(vertices, size);
		}

		return nullptr;
	}
}