#pragma once

#include "Engine/API/Rendering/ColorBlendAttachment.hpp"
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
	struct ColorBlendState {
		std::vector<ColorBlendAttachment> attachments;
	};

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
		CullMode cullMode;
		FrontFace frontFace;
		float32 lineWidth;
		float32 minSampleShading;
		ColorBlendState colorBlendState;
		DepthStencilState depthStencilState;
		DynamicState dynamicState;
	};
}