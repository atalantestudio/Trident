// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/VK/ComputePipeline.hpp"
#include "Engine/Adapter/Graphics/VK/Device.hpp"
#include "Engine/Adapter/Graphics/VK/PipelineLayout.hpp"
#include "Engine/Adapter/Graphics/VK/ShaderModule.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace Trident {
	VK::ComputePipeline::ComputePipeline(const Trident::Device* device) :
		Trident::ComputePipeline(device)
	{}

	ref<Trident::ComputePipeline> VK::createComputePipeline(const Trident::Device* device, const ComputePipelineDescriptor& descriptor) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const VK::Device* _device = static_cast<const VK::Device*>(device);
		VkResult result;

		ref<VK::ComputePipeline> computePipeline = createRef<VK::ComputePipeline>(device);

		VkComputePipelineCreateInfo computePipelineCreateInfo{};
		computePipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
		computePipelineCreateInfo.pNext = nullptr;
		computePipelineCreateInfo.flags = 0;
		computePipelineCreateInfo.stage = std::static_pointer_cast<VK::ShaderModule>(descriptor.computeShaderModule)->createInfo;
		computePipelineCreateInfo.layout = std::static_pointer_cast<VK::PipelineLayout>(descriptor.pipelineLayout)->pipelineLayout;
		computePipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
		computePipelineCreateInfo.basePipelineIndex = 0;

		result = _device->vkCreateComputePipelines(_device->device, VK_NULL_HANDLE, 1, &computePipelineCreateInfo, nullptr, &computePipeline->pipeline);

		ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to create compute pipeline ([]).", string_VkResult(result));

		#if defined(BUILD_MODE_DEBUG)
			if (descriptor.name.count() > 0) {
				VkDebugUtilsObjectNameInfoEXT debugUtilsObjectNameInfo{};
				debugUtilsObjectNameInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
				debugUtilsObjectNameInfo.pNext = nullptr;
				debugUtilsObjectNameInfo.objectType = VK_OBJECT_TYPE_PIPELINE;
				debugUtilsObjectNameInfo.objectHandle = (uint64) computePipeline->pipeline;
				debugUtilsObjectNameInfo.pObjectName = &descriptor.name[0];

				result = _device->vkSetDebugUtilsObjectNameEXT(_device->device, &debugUtilsObjectNameInfo);

				ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to set object name ([]).", string_VkResult(result));
			}
		#endif

		return computePipeline;
	}

	void VK::destroyComputePipeline(const Trident::Device* device, const Trident::ComputePipeline* computePipeline) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const VK::Device* _device = static_cast<const VK::Device*>(device);
		const VK::ComputePipeline* _computePipeline = static_cast<const VK::ComputePipeline*>(computePipeline);

		_device->vkDestroyPipeline(_device->device, _computePipeline->pipeline, nullptr);
	}
}