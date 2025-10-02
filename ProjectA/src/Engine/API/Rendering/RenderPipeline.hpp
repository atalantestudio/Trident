#pragma once

#include "Engine/API/Rendering/Descriptor/RenderPipelineDescriptor.hpp"
#include "Engine/Validation/Descriptor.hpp"

namespace ProjectA {
	class RenderPipeline {
		public:
			virtual ~RenderPipeline() = default;

		protected:
			explicit RenderPipeline(const RenderPipelineDescriptor& descriptor) {
				DEBUG_DESCRIPTOR_GUARD(descriptor);
			}
	};
}