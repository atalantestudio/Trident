#include "pch.hpp"

#include "Platform/OpenGL/OpenGLShader.hpp"
#include "ProjectA/Renderer/Renderer.hpp"
#include "ProjectA/Renderer/Shader.hpp"

namespace ProjectA {
	Ref<Shader> Shader::create(const std::string& vertexShaderSource, const std::string& fragmentShaderSource, const std::string& name) {
		switch (Renderer::getGraphicsAPI()) {
			case RendererAPI::GraphicsAPI::None:
				CORE_ASSERT(false, "Graphics::None is not supported yet.")

				return nullptr;

			case RendererAPI::GraphicsAPI::OpenGL:
				return std::make_shared<OpenGLShader>(vertexShaderSource, fragmentShaderSource, name);
		}

		return nullptr;
	}

	Ref<Shader> Shader::create(const std::string& path) {
		switch (Renderer::getGraphicsAPI()) {
			case RendererAPI::GraphicsAPI::None:
				CORE_ASSERT(false, "Graphics::None is not supported yet.")

				return nullptr;

			case RendererAPI::GraphicsAPI::OpenGL:
				return std::make_shared<OpenGLShader>(path);
		}

		return nullptr;
	}
}