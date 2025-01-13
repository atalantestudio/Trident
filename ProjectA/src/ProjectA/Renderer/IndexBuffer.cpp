#include "pch.hpp"

#include "Platform/OpenGL/OpenGLIndexBuffer.hpp"
#include "ProjectA/Renderer/IndexBuffer.hpp"
#include "ProjectA/Renderer/Renderer.hpp"

namespace ProjectA {
	ref<IndexBuffer> IndexBuffer::create(const uint32_t* indices, uint32_t count) {
		switch (Renderer::getGraphicsAPI()) {
			case RendererAPI::GraphicsAPI::None:
				ENGINE_ASSERT(false, "Graphics::None is not supported yet.")

				return nullptr;

			case RendererAPI::GraphicsAPI::OpenGL:
				return createRef<OpenGLIndexBuffer>(indices, count);
		}

		return nullptr;
	}
}