#include "pch.hpp"

#include "Platform/OpenGL/OpenGLTexture2D.hpp"

#include "ProjectA/Renderer/Renderer.hpp"
#include "ProjectA/Renderer/Texture2D.hpp"

namespace ProjectA {
	ref<Texture2D> Texture2D::create(std::size_t width, std::size_t height) {
		switch (Renderer::getGraphicsAPI()) {
			case RendererAPI::GraphicsAPI::None:
				ENGINE_ASSERT(false, "Graphics::None is not supported yet.")

				return nullptr;

			case RendererAPI::GraphicsAPI::OpenGL:
				return createRef<OpenGLTexture2D>(width, height);
		}

		return nullptr;
	}

	ref<Texture2D> Texture2D::create(const std::string& path) {
		switch (Renderer::getGraphicsAPI()) {
			case RendererAPI::GraphicsAPI::None:
				ENGINE_ASSERT(false, "Graphics::None is not supported yet.")

				return nullptr;

			case RendererAPI::GraphicsAPI::OpenGL:
				return createRef<OpenGLTexture2D>(path);
		}

		return nullptr;
	}
}