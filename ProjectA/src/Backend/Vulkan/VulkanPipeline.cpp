#include "pch.hpp"

#include "Backend/Vulkan/VulkanPipeline.hpp"

namespace ProjectA {
	VulkanPipeline::VulkanPipeline(VkPipelineBindPoint bindPoint, const VulkanPipelineLayout* layout) :
		bindPoint(bindPoint),
		layout(layout)
	{}
}