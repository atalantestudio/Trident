// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/VK/ComputePipeline.hpp"
#include "Engine/Adapter/Graphics/VK/PipelineLayout.hpp"
#include "Engine/Adapter/Graphics/VK/ShaderModule.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace ProjectA {
	VK::ComputePipeline::ComputePipeline(const ComputePipelineDescriptor& descriptor, const VK::Device& device) :
		ProjectA::ComputePipeline(descriptor),
		VK::Pipeline(VK_PIPELINE_BIND_POINT_COMPUTE),
		device(device)
	{
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		VkComputePipelineCreateInfo computePipelineCreateInfo{};
		computePipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
		computePipelineCreateInfo.pNext = nullptr;
		computePipelineCreateInfo.flags = 0;
		computePipelineCreateInfo.stage = std::static_pointer_cast<VK::ShaderModule>(descriptor.computeShaderModule)->createInfo;
		computePipelineCreateInfo.layout = std::static_pointer_cast<VK::PipelineLayout>(descriptor.pipelineLayout)->pipelineLayout;
		computePipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
		computePipelineCreateInfo.basePipelineIndex = 0;

		const VkResult result = vkCreateComputePipelines(device.device, VK_NULL_HANDLE, 1, &computePipelineCreateInfo, nullptr, &pipeline);

		ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to create compute pipeline ([]).", string_VkResult(result));
	}

	VK::ComputePipeline::~ComputePipeline() {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		vkDestroyPipeline(device.device, pipeline, nullptr);
	}
}