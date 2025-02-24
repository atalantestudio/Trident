#include "pch.hpp"

#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/RenderPipeline.hpp"

#include "Platform/OpenGL/OpenGLRenderPipeline.hpp"

namespace ProjectA {
	ref<RenderPipeline> RenderPipeline::create(const RenderPipelineDescriptor& descriptor) {
		switch (Renderer::getGraphicsAPI()) {
			case RendererAPI::GraphicsAPI::None:
				ENGINE_ASSERT(false, "Graphics::None is not supported yet.");

				return nullptr;

			case RendererAPI::GraphicsAPI::OpenGL:
				return createRef<OpenGLRenderPipeline>(descriptor);

			case RendererAPI::GraphicsAPI::Vulkan:
				ENGINE_ASSERT(false, "Graphics::Vulkan is not supported yet.");

				return nullptr;
		}

		return nullptr;
	}
}