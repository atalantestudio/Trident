#pragma once

#include "Engine/Adapter/Graphics/VK/Pipeline.hpp"

#include "Engine/API/Rendering/ComputePipeline.hpp"

namespace ProjectA {
	class VulkanComputePipeline final : public VulkanPipeline, public ComputePipeline {
		public:
			explicit VulkanComputePipeline(VkDevice device, const ComputePipelineDescriptor& descriptor);

			~VulkanComputePipeline() final override;

		private:
			const VkDevice device;
	};
}