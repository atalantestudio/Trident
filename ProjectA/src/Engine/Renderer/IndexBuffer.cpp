#include "pch.hpp"

#include "Platform/OpenGL/OpenGLIndexBuffer.hpp"

#include "Engine/Renderer/IndexBuffer.hpp"
#include "Engine/Renderer/Renderer.hpp"

namespace ProjectA {
	ref<IndexBuffer> IndexBuffer::create(const uint32* indices, uint32 count) {
		switch (Renderer::getGraphicsApi()) {
			case GraphicsAPI::NONE:
				ENGINE_ASSERT(false, "NONE is not supported yet.");

				return nullptr;
			case GraphicsAPI::OPENGL:
				return createRef<OpenGLIndexBuffer>(indices, count);
			case GraphicsAPI::VULKAN:
				ENGINE_ASSERT(false, "VULKAN is not supported yet.");

				return nullptr;
		}

		return nullptr;
	}
}