// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/VK/ComputePipeline.hpp"
#include "Engine/Adapter/Graphics/VK/PipelineLayout.hpp"
#include "Engine/Adapter/Graphics/VK/ShaderModule.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace Trident {
	VK::ComputePipeline::ComputePipeline(const ComputePipelineDescriptor& descriptor, const VK::Device& device) :
		Trident::ComputePipeline(descriptor),
		VK::Pipeline(VK_PIPELINE_BIND_POINT_COMPUTE),
		device(device)
	{
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		VkResult result;

		VkComputePipelineCreateInfo computePipelineCreateInfo{};
		computePipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
		computePipelineCreateInfo.pNext = nullptr;
		computePipelineCreateInfo.flags = 0;
		computePipelineCreateInfo.stage = std::static_pointer_cast<VK::ShaderModule>(descriptor.computeShaderModule)->createInfo;
		computePipelineCreateInfo.layout = std::static_pointer_cast<VK::PipelineLayout>(descriptor.pipelineLayout)->pipelineLayout;
		computePipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
		computePipelineCreateInfo.basePipelineIndex = 0;

		result = vkCreateComputePipelines(device.device, VK_NULL_HANDLE, 1, &computePipelineCreateInfo, nullptr, &pipeline);

		ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to create compute pipeline ([]).", string_VkResult(result));

		#if defined(BUILD_MODE_DEBUG)
			if (descriptor.name.count() > 0) {
				VkDebugUtilsObjectNameInfoEXT debugUtilsObjectNameInfo{};
				debugUtilsObjectNameInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
				debugUtilsObjectNameInfo.pNext = nullptr;
				debugUtilsObjectNameInfo.objectType = VK_OBJECT_TYPE_PIPELINE;
				debugUtilsObjectNameInfo.objectHandle = (uint64) pipeline;
				debugUtilsObjectNameInfo.pObjectName = &descriptor.name[0];

				result = vkSetDebugUtilsObjectNameEXT(device.device, &debugUtilsObjectNameInfo);

				ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to set object name ([]).", string_VkResult(result));
			}
		#endif
	}

	VK::ComputePipeline::~ComputePipeline() {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		vkDestroyPipeline(device.device, pipeline, nullptr);
	}
}