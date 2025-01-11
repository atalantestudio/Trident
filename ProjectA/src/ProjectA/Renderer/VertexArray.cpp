#include "pch.hpp"

#include "Platform/OpenGL/OpenGLVertexArray.hpp"
#include "ProjectA/Renderer/Renderer.hpp"
#include "ProjectA/Renderer/VertexArray.hpp"

namespace ProjectA {
	Ref<VertexArray> VertexArray::create() {
		switch (Renderer::getGraphicsAPI()) {
			case RendererAPI::GraphicsAPI::None:
				CORE_ASSERT(false, "Graphics::None is not supported yet.")

				return nullptr;

			case RendererAPI::GraphicsAPI::OpenGL:
				return std::make_shared<OpenGLVertexArray>();
		}

		return nullptr;
	}

	VertexArray::~VertexArray() {}
}