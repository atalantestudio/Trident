#include "pch.hpp"
#include "Platform/OpenGL/OpenGLTexture2D.hpp"
#include "Renderer.hpp"
#include "Texture2D.hpp"

namespace ProjectA {
	Ref<Texture2D> Texture2D::create(const std::string& path) {
		switch (Renderer::getGraphicsAPI()) {
			case RendererAPI::GraphicsAPI::None:
				CORE_ASSERT(false, "Graphics::None is not supported yet.")

				return nullptr;

			case RendererAPI::GraphicsAPI::OpenGL:
				return std::make_shared<OpenGLTexture2D>(path);
;		}
	}
}