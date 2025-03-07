#include "pch.hpp"

#include "Backend/Vulkan/VulkanShaderModule.hpp"

namespace ProjectA {
	VulkanShaderModule::VulkanShaderModule(const VkDevice device, const VkShaderStageFlagBits stage, const std::string& code) :
		device(device)
	{
		PROFILE_SCOPE(FUNC_SIG);

		ENGINE_TRACE("[VulkanShaderModule]");

		VkShaderModuleCreateInfo shaderModuleCreateInfo{};
		shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		shaderModuleCreateInfo.pNext = nullptr;
		shaderModuleCreateInfo.flags = 0;
		shaderModuleCreateInfo.codeSize = code.size();
		shaderModuleCreateInfo.pCode = reinterpret_cast<const uint32*>(code.c_str());

		VkShaderModule shaderModule;

		if (vkCreateShaderModule(device, &shaderModuleCreateInfo, nullptr, &shaderModule) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create Vulkan shader module.");
		}

		object.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		object.pNext = nullptr;
		object.flags = 0;
		object.stage = stage;
		object.module = shaderModule;
		object.pName = "main";
		object.pSpecializationInfo = nullptr;
	}

	VulkanShaderModule::~VulkanShaderModule() {
		PROFILE_SCOPE(FUNC_SIG);

		ENGINE_TRACE("[~VulkanShaderModule]");

		vkDestroyShaderModule(device, object.module, nullptr);
	}
}