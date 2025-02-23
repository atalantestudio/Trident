#include "pch.hpp"

#include "Platform/OpenGL/OpenGLShaderModule.hpp"

#include "ProjectA/Renderer/Renderer.hpp"
#include "ProjectA/Renderer/ShaderModule.hpp"

namespace ProjectA {
	ref<ShaderModule> ShaderModule::create(const ShaderStage stage, const std::string& code) {
		switch (Renderer::getGraphicsAPI()) {
			case RendererAPI::GraphicsAPI::None:
				ENGINE_ASSERT(false, "Graphics::None is not supported yet.");

				return nullptr;

			case RendererAPI::GraphicsAPI::OpenGL:
				return createRef<OpenGLShaderModule>(stage, code);

			case RendererAPI::GraphicsAPI::Vulkan:
				ENGINE_ASSERT(false, "Graphics::Vulkan is not supported yet.");

				return nullptr;
		}

		return nullptr;
	}
}