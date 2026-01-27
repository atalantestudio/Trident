// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

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