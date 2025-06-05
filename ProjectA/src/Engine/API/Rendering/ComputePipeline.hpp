#pragma once

#include "Engine/API/Rendering/Pipeline.hpp"
#include "Engine/API/Rendering/PipelineLayout.hpp"
#include "Engine/API/Rendering/ShaderModule.hpp"

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