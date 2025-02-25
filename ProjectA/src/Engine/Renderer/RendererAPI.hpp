#pragma once

#include "Engine/Renderer/RenderPipeline.hpp"
#include "Engine/Renderer/ShaderStage.hpp"

namespace ProjectA {
	class RendererAPI {
		public:
			virtual ~RendererAPI() = default;

			virtual void setViewport(int32 x, int32, uint32 width, uint32 height) const = 0;

			virtual void setClearColor(const vec4<float32>& color) const = 0;

			virtual void clear() const = 0;

			virtual void drawIndexed(const ref<RenderPipeline>& renderPipeline, uint32 indexCount) const = 0;

			// --- Graphics API provided by the engine ---

			virtual ref<ShaderModule> createShaderModule(const ShaderStage stage, const std::string& code) const = 0;

			virtual ref<RenderPipeline> createRenderPipeline(const RenderPipelineDescriptor& descriptor) const = 0;
	};
}