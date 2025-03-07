#pragma once

#include "Engine/Renderer/Buffer/BufferLayout.hpp"
#include "Engine/Renderer/CullMode.hpp"
#include "Engine/Renderer/FrontFace.hpp"
#include "Engine/Renderer/PipelineLayout.hpp"
#include "Engine/Renderer/PrimitiveTopology.hpp"
#include "Engine/Renderer/RenderPass.hpp"
#include "Engine/Renderer/ShaderModule.hpp"
#include "Engine/Renderer/Swapchain.hpp"

namespace ProjectA {
	struct RenderPipelineDescriptor {
		ref<RenderPass> renderPass;
		ref<Swapchain> swapchain;
		ref<PipelineLayout> pipelineLayout;
		BufferLayout vertexBufferLayout;
		ref<ShaderModule> vertexShaderModule;
		ref<ShaderModule> fragmentShaderModule;
		PrimitiveTopology primitiveTopology;
		CullMode cullMode;
		FrontFace frontFace;
	};

	class RenderPipeline {
		public:
			virtual ~RenderPipeline() = default;
	};
}