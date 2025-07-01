#pragma once

#include "Engine/API/Rendering/Pipeline.hpp"
#include "Engine/API/Rendering/Descriptor/ComputePipelineDescriptor.hpp"
#include "Engine/Validation/Descriptor.hpp"

namespace ProjectA {
	class ComputePipeline : public Pipeline {
		public:
			virtual ~ComputePipeline() = default;

		protected:
			explicit ComputePipeline(const ComputePipelineDescriptor& descriptor) {
				DEBUG_DESCRIPTOR_GUARD(descriptor);
			}
	};
}