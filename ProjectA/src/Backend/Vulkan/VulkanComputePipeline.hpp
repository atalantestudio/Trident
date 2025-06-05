#pragma once

#include "Backend/Vulkan/VulkanPipeline.hpp"

#include "Engine/API/Rendering/ComputePipeline.hpp"
#include "Engine/API/Rendering/Descriptor/ComputePipelineDescriptor.hpp"

namespace ProjectA {
	class VulkanComputePipeline : public ComputePipeline, public VulkanPipeline {
		public:
			VulkanComputePipeline(VkDevice device, const ComputePipelineDescriptor& descriptor);

			~VulkanComputePipeline();

		private:
			const VkDevice device;
	};
}