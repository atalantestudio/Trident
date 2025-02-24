#include "pch.hpp"

#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/RenderPipeline.hpp"

#include "Platform/OpenGL/OpenGLRenderPipeline.hpp"

namespace ProjectA {
	ref<RenderPipeline> RenderPipeline::create(const RenderPipelineDescriptor& descriptor) {
		switch (Renderer::getGraphicsApi()) {
			case GraphicsAPI::NONE:
				ENGINE_ASSERT(false, "NONE is not supported yet.");

				return nullptr;
			case GraphicsAPI::OPENGL:
				return createRef<OpenGLRenderPipeline>(descriptor);
			case GraphicsAPI::VULKAN:
				ENGINE_ASSERT(false, "VULKAN is not supported yet.");

				return nullptr;
		}

		return nullptr;
	}
}