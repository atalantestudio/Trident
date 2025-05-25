#pragma once

#include "Engine/API/Rendering/ShaderModule.hpp"
#include "Engine/Renderer/Pipeline.hpp"
#include "Engine/Renderer/PipelineLayout.hpp"

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