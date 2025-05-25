#pragma once

#include "Engine/API/Rendering/CompareOperation.hpp"
#include "Engine/API/Rendering/RenderPass.hpp"
#include "Engine/API/Rendering/ShaderModule.hpp"
#include "Engine/Renderer/CullMode.hpp"
#include "Engine/Renderer/DynamicState.hpp"
#include "Engine/Renderer/FrontFace.hpp"
#include "Engine/Renderer/PipelineLayout.hpp"
#include "Engine/Renderer/PolygonMode.hpp"
#include "Engine/Renderer/PrimitiveTopology.hpp"
#include "Engine/Renderer/VertexInputBinding.hpp"

namespace ProjectA {
	struct DepthStencilState {
		bool depthTest;
		bool depthWrite;
		CompareOperation compareOperation;
	};

	struct RenderPipelineDescriptor {
		ref<RenderPass> renderPass;
		ref<PipelineLayout> pipelineLayout;
		std::vector<VertexInputBinding> vertexInputBindings;
		ref<ShaderModule> vertexShaderModule;
		ref<ShaderModule> fragmentShaderModule;
		PrimitiveTopology primitiveTopology;
		PolygonMode polygonMode;
		CullMode cullMode = CullMode::BACK;
		FrontFace frontFace = FrontFace::CLOCKWISE;
		float32 lineWidth = 1.0f;
		float32 minSampleShading = 0.0f;
		DepthStencilState depthStencilState;
		DynamicState dynamicState;
	};
}