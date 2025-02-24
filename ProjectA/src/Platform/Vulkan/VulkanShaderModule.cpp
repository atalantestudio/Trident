#include "pch.hpp"

#include <vulkan/vulkan.h>

#include "Platform/Vulkan/VulkanShaderModule.hpp"

namespace ProjectA {
	VulkanShaderModule::VulkanShaderModule(const VkDevice& device, const VkShaderStageFlagBits stage, const std::string& code) : ObjectWrapper(nullptr), device(device) {
		VkShaderModuleCreateInfo shaderModuleCreateInfo{};
		shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		shaderModuleCreateInfo.pNext = nullptr;
		shaderModuleCreateInfo.flags = 0;
		shaderModuleCreateInfo.codeSize = code.size();
		shaderModuleCreateInfo.pCode = reinterpret_cast<const uint32*>(code.c_str());

		if (vkCreateShaderModule(device, &shaderModuleCreateInfo, nullptr, &object) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create shader module.");
		}

		// TODO: Store pipeline shader stage.
		VkPipelineShaderStageCreateInfo pipelineShaderStageCreateInfo{};
		pipelineShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		pipelineShaderStageCreateInfo.pNext = nullptr;
		pipelineShaderStageCreateInfo.flags = 0;
		pipelineShaderStageCreateInfo.stage = stage;
		pipelineShaderStageCreateInfo.module = object;
		pipelineShaderStageCreateInfo.pName = "main";
		pipelineShaderStageCreateInfo.pSpecializationInfo = nullptr;
	}

	VulkanShaderModule::~VulkanShaderModule() {
		vkDestroyShaderModule(device, object, nullptr);
	}
}