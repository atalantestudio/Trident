#include "pch.hpp"
#include "Platform/OpenGL/OpenGLVertexArray.hpp"
#include "Renderer.hpp"
#include "VertexArray.hpp"

namespace ProjectA {
	VertexArray* VertexArray::create() {
		switch (Renderer::getGraphicsAPI()) {
			case RendererAPI::GraphicsAPI::None:
				CORE_ASSERT(false, "Graphics::None is not supported yet.")

				return nullptr;

			case RendererAPI::GraphicsAPI::OpenGL:
				return new OpenGLVertexArray();
;		}
	}

	VertexArray::~VertexArray() {}
}