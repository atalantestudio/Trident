#include "pch.hpp"

#include "Backend/Vulkan/VulkanComputePipeline.hpp"
#include "Backend/Vulkan/VulkanShaderModule.hpp"

namespace ProjectA {
	VulkanComputePipeline::VulkanComputePipeline(VkDevice device, const ComputePipelineDescriptor& descriptor) :
		VulkanPipeline(VK_PIPELINE_BIND_POINT_COMPUTE, std::static_pointer_cast<VulkanPipelineLayout>(descriptor.pipelineLayout).get()),
		device(device)
	{
		PROFILE_SCOPE(FUNC_SIG);

		VkComputePipelineCreateInfo computePipelineCreateInfo{};
		computePipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
		computePipelineCreateInfo.pNext = nullptr;
		computePipelineCreateInfo.flags = 0;
		computePipelineCreateInfo.stage = std::static_pointer_cast<VulkanShaderModule>(descriptor.computeShaderModule)->object;
		computePipelineCreateInfo.layout = layout->object;
		computePipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
		computePipelineCreateInfo.basePipelineIndex = 0;

		const VkResult result = vkCreateComputePipelines(device, VK_NULL_HANDLE, 1, &computePipelineCreateInfo, nullptr, &object);

		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to create Vulkan compute pipeline ([]).", string_VkResult(result));
	}

	VulkanComputePipeline::~VulkanComputePipeline() {
		PROFILE_SCOPE(FUNC_SIG);

		vkDestroyPipeline(device, object, nullptr);
	}
}