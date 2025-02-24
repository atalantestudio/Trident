#include "pch.hpp"

#include "Platform/OpenGL/OpenGLShaderModule.hpp"

#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/ShaderModule.hpp"

namespace ProjectA {
	ref<ShaderModule> ShaderModule::create(const ShaderStage stage, const std::string& code) {
		switch (Renderer::getGraphicsApi()) {
			case GraphicsAPI::NONE:
				ENGINE_ASSERT(false, "NONE is not supported yet.");

				return nullptr;
			case GraphicsAPI::OPENGL:
				return createRef<OpenGLShaderModule>(stage, code);
			case GraphicsAPI::VULKAN:
				ENGINE_ASSERT(false, "VULKAN is not supported yet.");

				return nullptr;
		}

		return nullptr;
	}
}