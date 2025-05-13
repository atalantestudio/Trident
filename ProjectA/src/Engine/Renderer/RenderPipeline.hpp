#pragma once

#include "Engine/Renderer/CullMode.hpp"
#include "Engine/Renderer/DynamicState.hpp"
#include "Engine/Renderer/FrontFace.hpp"
#include "Engine/Renderer/Pipeline.hpp"
#include "Engine/Renderer/PolygonMode.hpp"
#include "Engine/Renderer/PrimitiveTopology.hpp"
#include "Engine/Renderer/RenderPass.hpp"
#include "Engine/Renderer/ShaderModule.hpp"
#include "Engine/Renderer/VertexInputBinding.hpp"

namespace ProjectA {
	struct RenderPipelineDescriptor {
		ref<RenderPass> renderPass;
		ref<PipelineLayout> pipelineLayout;
		chain<VertexInputBinding> vertexInputBindings;
		ref<ShaderModule> vertexShaderModule;
		ref<ShaderModule> fragmentShaderModule;
		PrimitiveTopology primitiveTopology;
		PolygonMode polygonMode;
		CullMode cullMode;
		FrontFace frontFace;

		// Line width, used by render pipelines having the LINE polygon mode.
		float32 lineWidth = 1.0f;

		DynamicState dynamicState;
	};

	class RenderPipeline : public Pipeline {
		public:
			virtual ~RenderPipeline() = default;
	};
}