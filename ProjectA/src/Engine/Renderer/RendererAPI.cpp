#include "pch.hpp"

#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/RendererAPI.hpp"

#include "Platform/OpenGL/OpenGLRendererAPI.hpp"

#include "Platform/Vulkan/VulkanRendererAPI.hpp"

namespace ProjectA {
	scope<RendererAPI> RendererAPI::create() {
		switch (Renderer::getGraphicsApi()) {
			case GraphicsAPI::NONE:
				ENGINE_ASSERT(false, "NONE is not supported yet.");

				return nullptr;
			case GraphicsAPI::OPENGL:
				return createScope<OpenGLRendererAPI>();
			case GraphicsAPI::VULKAN:
				return createScope<VulkanRendererAPI>();
		}

		return nullptr;
	}
}