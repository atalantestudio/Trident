#pragma once

#include "Engine/API/Rendering/Descriptor/ComputePipelineDescriptor.hpp"
#include "Engine/Validation/Descriptor.hpp"

namespace Trident {
	class ComputePipeline {
		public:
			virtual ~ComputePipeline() = default;

		protected:
			explicit ComputePipeline(const ComputePipelineDescriptor& descriptor) {
				DEBUG_DESCRIPTOR_GUARD(descriptor);
			}
	};
}