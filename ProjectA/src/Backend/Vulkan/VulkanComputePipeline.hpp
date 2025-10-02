#pragma once

#include "Backend/Vulkan/VulkanPipeline.hpp"

#include "Engine/API/Rendering/ComputePipeline.hpp"

namespace ProjectA {
	class VulkanComputePipeline final : public VulkanPipeline, public ComputePipeline {
		public:
			explicit VulkanComputePipeline(const VkDevice device, const ComputePipelineDescriptor& descriptor);

			~VulkanComputePipeline() final override;

		private:
			const VkDevice device;
	};
}