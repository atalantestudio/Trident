#pragma once

#include "Engine/API/Rendering/RenderPass.hpp"
#include "Engine/Renderer/CullMode.hpp"
#include "Engine/Renderer/DynamicState.hpp"
#include "Engine/Renderer/FrontFace.hpp"
#include "Engine/Renderer/Pipeline.hpp"
#include "Engine/Renderer/PolygonMode.hpp"
#include "Engine/Renderer/PrimitiveTopology.hpp"
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
		CullMode cullMode = CullMode::BACK;
		FrontFace frontFace = FrontFace::CLOCKWISE;
		float32 lineWidth = 1.0f;
		float32 minSampleShading = 0.0f;
		DynamicState dynamicState;
	};

	class RenderPipeline : public Pipeline {
		public:
			virtual ~RenderPipeline() = default;
	};
}