// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/VK/Device.hpp"
#include "Engine/API/Rendering/ComputePipeline.hpp"

namespace Trident {
	class VK::ComputePipeline final : public Trident::ComputePipeline {
		public:
			explicit ComputePipeline(const Trident::Device* device);

		public:
			VkPipeline pipeline;
	};

	namespace VK {
		ref<Trident::ComputePipeline> createComputePipeline(const Trident::Device* device, const ComputePipelineDescriptor& descriptor);

		void destroyComputePipeline(const Trident::Device* device, const Trident::ComputePipeline* computePipeline);
	}
}