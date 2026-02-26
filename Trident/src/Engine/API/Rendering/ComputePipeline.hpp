// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/Descriptor/ComputePipelineDescriptor.hpp"
#include "Engine/API/Rendering/Device.hpp"

namespace Trident {
	class ComputePipeline {
		public:
			~ComputePipeline();

		protected:
			explicit ComputePipeline(const Device* device);

		public:
			const Device* device;
	};
}