// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/VK/Device.hpp"
#include "Engine/Adapter/Graphics/VK/Pipeline.hpp"
#include "Engine/API/Rendering/ComputePipeline.hpp"

namespace ProjectA {
	class VK::ComputePipeline final : public VK::Pipeline, public ProjectA::ComputePipeline {
		public:
			explicit ComputePipeline(const ComputePipelineDescriptor& descriptor, const VK::Device& device);

			~ComputePipeline() final override;

		private:
			const VK::Device& device;
	};
}