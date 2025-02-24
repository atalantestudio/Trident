#include "pch.hpp"

#include "Platform/OpenGL/OpenGLVertexBuffer.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/VertexBuffer.hpp"

namespace ProjectA {
	ref<VertexBuffer> VertexBuffer::create(uint32 size) {
		switch (Renderer::getGraphicsApi()) {
			case GraphicsAPI::NONE:
				ENGINE_ASSERT(false, "NONE is not supported yet.");

				return nullptr;
			case GraphicsAPI::OPENGL:
				return createRef<OpenGLVertexBuffer>(size);
			case GraphicsAPI::VULKAN:
				ENGINE_ASSERT(false, "VULKAN is not supported yet.");

				return nullptr;
		}

		return nullptr;
	}

	ref<VertexBuffer> VertexBuffer::create(float32* vertices, uint32 size) {
		switch (Renderer::getGraphicsApi()) {
			case GraphicsAPI::NONE:
				ENGINE_ASSERT(false, "NONE is not supported yet.");

				return nullptr;
			case GraphicsAPI::OPENGL:
				return createRef<OpenGLVertexBuffer>(vertices, size);
			case GraphicsAPI::VULKAN:
				ENGINE_ASSERT(false, "VULKAN is not supported yet.");

				return nullptr;
		}

		return nullptr;
	}
}