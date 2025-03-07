#include "pch.hpp"

#include "Engine/Renderer/Context.hpp"
#include "Engine/Renderer/Renderer.hpp"

#include "Backend/OpenGL/OpenGLContext.hpp"
#include "Backend/Vulkan/VulkanContext.hpp"

namespace ProjectA {
	scope<Context> Context::create(void* window) {
		switch (Renderer::getGraphicsApi()) {
			case GraphicsAPI::NONE:
				ENGINE_ASSERT(false, "NONE is not supported yet.");

				return nullptr;
			case GraphicsAPI::OPENGL:
				return createScope<OpenGLContext>(static_cast<GLFWwindow*>(window));
			case GraphicsAPI::VULKAN:
				return createScope<VulkanContext>(static_cast<GLFWwindow*>(window));
		}

		return nullptr;
	}
}