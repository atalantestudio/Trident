#include "pch.hpp"
#include "Platform/OpenGL/OpenGLIndexBuffer.hpp"
#include "Renderer.hpp"
#include "IndexBuffer.hpp"

namespace ProjectA {
	Ref<IndexBuffer> IndexBuffer::create(uint32_t* indices, uint32_t count) {
		switch (Renderer::getGraphicsAPI()) {
			case RendererAPI::GraphicsAPI::None:
				CORE_ASSERT(false, "Graphics::None is not supported yet.")

				return nullptr;

			case RendererAPI::GraphicsAPI::OpenGL:
				return std::make_shared<OpenGLIndexBuffer>(indices, count);
		}

		return nullptr;
	}
}