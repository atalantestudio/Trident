#include "pch.hpp"

#include "Platform/OpenGL/OpenGLTexture2D.hpp"

#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Texture2D.hpp"

namespace ProjectA {
	ref<Texture2D> Texture2D::create(std::size_t width, std::size_t height) {
		switch (Renderer::getGraphicsApi()) {
			case GraphicsAPI::NONE:
				ENGINE_ASSERT(false, "Graphics::NONE is not supported yet.");

				return nullptr;

			case GraphicsAPI::OPENGL:
				return createRef<OpenGLTexture2D>(static_cast<GLsizei>(width), static_cast<GLsizei>(height));

			case GraphicsAPI::VULKAN:
				ENGINE_ASSERT(false, "Graphics::VULKAN is not supported yet.");

				return nullptr;
		}

		return nullptr;
	}

	ref<Texture2D> Texture2D::create(const std::string& path) {
		switch (Renderer::getGraphicsApi()) {
			case GraphicsAPI::NONE:
				ENGINE_ASSERT(false, "Graphics::NONE is not supported yet.");

				return nullptr;

			case GraphicsAPI::OPENGL:
				return createRef<OpenGLTexture2D>(path);

			case GraphicsAPI::VULKAN:
				ENGINE_ASSERT(false, "Graphics::VULKAN is not supported yet.");

				return nullptr;
		}

		return nullptr;
	}
}