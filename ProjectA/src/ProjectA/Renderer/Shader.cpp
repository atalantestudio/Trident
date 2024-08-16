#include "pch.hpp"
#include "Platform/OpenGL/OpenGLShader.hpp"
#include "Renderer.hpp"
#include "Shader.hpp"

namespace ProjectA {
	Shader* Shader::create(const std::string& vertexShader, const std::string& fragmentShader) {
		switch (Renderer::getGraphicsAPI()) {
			case RendererAPI::GraphicsAPI::None:
				CORE_ASSERT(false, "Graphics::None is not supported yet.")

				return nullptr;

			case RendererAPI::GraphicsAPI::OpenGL:
				return new OpenGLShader(vertexShader, fragmentShader);
		}
	}
}