#pragma once

#include "Engine/API/Rendering/Descriptor/RenderPipelineDescriptor.hpp"
#include "Engine/Validation/Descriptor.hpp"

namespace ProjectA {
	class RenderPipeline {
		public:
			virtual ~RenderPipeline() = default;

		protected:
			// TODO: Used by the DX12 adapter (WIP).
			RenderPipeline() = default;

			explicit RenderPipeline(const RenderPipelineDescriptor& descriptor) {
				DEBUG_DESCRIPTOR_GUARD(descriptor);
			}
	};
}