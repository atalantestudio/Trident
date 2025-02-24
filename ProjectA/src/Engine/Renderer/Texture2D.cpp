#include "pch.hpp"

#include "Platform/OpenGL/OpenGLTexture2D.hpp"

#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Texture2D.hpp"

namespace ProjectA {
	ref<Texture2D> Texture2D::create(std::size_t width, std::size_t height) {
		switch (Renderer::getGraphicsAPI()) {
			case RendererAPI::GraphicsAPI::None:
				ENGINE_ASSERT(false, "Graphics::None is not supported yet.");

				return nullptr;

			case RendererAPI::GraphicsAPI::OpenGL:
				return createRef<OpenGLTexture2D>(static_cast<GLsizei>(width), static_cast<GLsizei>(height));

			case RendererAPI::GraphicsAPI::Vulkan:
				ENGINE_ASSERT(false, "Graphics::Vulkan is not supported yet.");

				return nullptr;
		}

		return nullptr;
	}

	ref<Texture2D> Texture2D::create(const std::string& path) {
		switch (Renderer::getGraphicsAPI()) {
			case RendererAPI::GraphicsAPI::None:
				ENGINE_ASSERT(false, "Graphics::None is not supported yet.");

				return nullptr;

			case RendererAPI::GraphicsAPI::OpenGL:
				return createRef<OpenGLTexture2D>(path);

			case RendererAPI::GraphicsAPI::Vulkan:
				ENGINE_ASSERT(false, "Graphics::Vulkan is not supported yet.");

				return nullptr;
		}

		return nullptr;
	}
}