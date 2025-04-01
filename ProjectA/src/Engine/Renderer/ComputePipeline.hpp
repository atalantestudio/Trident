#pragma once

#include "Engine/Renderer/Pipeline.hpp"
#include "Engine/Renderer/PipelineLayout.hpp"
#include "Engine/Renderer/ShaderModule.hpp"

namespace ProjectA {
	struct ComputePipelineDescriptor {
		ref<PipelineLayout> pipelineLayout;
		ref<ShaderModule> computeShaderModule;
	};

	class ComputePipeline : public Pipeline {
		public:
			virtual ~ComputePipeline() = default;
	};
}