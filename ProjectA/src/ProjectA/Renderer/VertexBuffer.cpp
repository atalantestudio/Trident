#include "pch.hpp"
#include "Platform/OpenGL/OpenGLVertexBuffer.hpp"
#include "Renderer.hpp"
#include "VertexBuffer.hpp"

namespace ProjectA {
	VertexBuffer* VertexBuffer::create(float* vertices, uint32_t size) {
		switch (Renderer::getGraphicsAPI()) {
			case RendererAPI::GraphicsAPI::None:
				CORE_ASSERT(false, "Graphics::None is not supported yet.")

				return nullptr;

			case RendererAPI::GraphicsAPI::OpenGL:
				return new OpenGLVertexBuffer(vertices, size);
		}

		return nullptr;
	}
}