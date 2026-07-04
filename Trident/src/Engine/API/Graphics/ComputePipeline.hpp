// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/Descriptor/ComputePipelineDescriptor.hpp"
#include "Engine/API/Graphics/Device.hpp"

namespace Trident {
	class ComputePipeline {
		public:
			~ComputePipeline();

		protected:
			explicit ComputePipeline(const Device* device);

		public:
			const Device* device;

			// TODO(graphics/pipeline/required-descriptor-sets)
			// Validate that the pipeline layout is bound before using the pipeline.
	};
}