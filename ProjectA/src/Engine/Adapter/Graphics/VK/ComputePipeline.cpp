#include "pch.hpp"

#include "Engine/Adapter/Graphics/VK/ComputePipeline.hpp"
#include "Engine/Adapter/Graphics/VK/PipelineLayout.hpp"
#include "Engine/Adapter/Graphics/VK/ShaderModule.hpp"

namespace ProjectA {
	VulkanComputePipeline::VulkanComputePipeline(VkDevice device, const ComputePipelineDescriptor& descriptor) :
		VulkanPipeline(VK_PIPELINE_BIND_POINT_COMPUTE),
		ComputePipeline(descriptor),
		device(device)
	{
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		VkComputePipelineCreateInfo computePipelineCreateInfo{};
		computePipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
		computePipelineCreateInfo.pNext = nullptr;
		computePipelineCreateInfo.flags = 0;
		computePipelineCreateInfo.stage = std::static_pointer_cast<VulkanShaderModule>(descriptor.computeShaderModule)->object;
		computePipelineCreateInfo.layout = std::static_pointer_cast<VulkanPipelineLayout>(descriptor.pipelineLayout)->object;
		computePipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
		computePipelineCreateInfo.basePipelineIndex = 0;

		const VkResult result = vkCreateComputePipelines(device, VK_NULL_HANDLE, 1, &computePipelineCreateInfo, nullptr, &object);

		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to create Vulkan compute pipeline ([]).", string_VkResult(result));
	}

	VulkanComputePipeline::~VulkanComputePipeline() {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		vkDestroyPipeline(device, object, nullptr);
	}
}