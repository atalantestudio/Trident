#pragma once

#include <vulkan/vulkan.h>

#include "Backend/Vulkan/VulkanPipeline.hpp"

#include "Engine/Renderer/ComputePipeline.hpp"

namespace ProjectA {
	class VulkanComputePipeline : public ComputePipeline, public VulkanPipeline {
		public:
			VulkanComputePipeline(VkDevice device, const ComputePipelineDescriptor& descriptor);

			~VulkanComputePipeline();

		private:
			const VkDevice device;
	};
}