// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/Descriptor/RenderPipelineDescriptor.hpp"
#include "Engine/Validation/Descriptor.hpp"

namespace Trident {
	class RenderPipeline {
		public:
			virtual ~RenderPipeline() = default;

		protected:
			explicit RenderPipeline(const RenderPipelineDescriptor& descriptor) {
				DEBUG_DESCRIPTOR_GUARD(descriptor);
			}
	};
}