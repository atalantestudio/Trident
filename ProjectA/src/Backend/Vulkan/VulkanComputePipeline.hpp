#pragma once

#include "Backend/Vulkan/VulkanPipeline.hpp"

#include "Engine/API/Rendering/ComputePipeline.hpp"
#include "Engine/API/Rendering/Descriptor/ComputePipelineDescriptor.hpp"

namespace ProjectA {
	class VulkanComputePipeline final : public ComputePipeline, public VulkanPipeline {
		public:
			explicit VulkanComputePipeline(VkDevice device, const ComputePipelineDescriptor& descriptor);

			~VulkanComputePipeline() final override;

		private:
			const VkDevice device;
	};
}