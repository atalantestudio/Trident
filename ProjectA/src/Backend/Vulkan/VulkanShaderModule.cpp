#include "pch.hpp"

#include "Backend/Vulkan/ShaderStage.hpp"
#include "Backend/Vulkan/VulkanShaderModule.hpp"

namespace ProjectA {
	VulkanShaderModule::VulkanShaderModule(VkDevice device, const chain<uint8>& code, const ShaderModuleDescriptor& descriptor) :
		device(device)
	{
		PROFILE_SCOPE(FUNC_SIG);

		VkShaderModuleCreateInfo shaderModuleCreateInfo{};
		shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		shaderModuleCreateInfo.pNext = nullptr;
		shaderModuleCreateInfo.flags = 0;
		shaderModuleCreateInfo.codeSize = code.size();
		shaderModuleCreateInfo.pCode = static_cast<const uint32*>(static_cast<const void*>(code.data()));

		VkShaderModule shaderModule;

		const VkResult result = vkCreateShaderModule(device, &shaderModuleCreateInfo, nullptr, &shaderModule);

		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to create Vulkan shader module (code: []).", result);

		object.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		object.pNext = nullptr;
		object.flags = 0;
		object.stage = getVkShaderStageFlagBits(descriptor.stage);
		object.module = shaderModule;
		object.pName = descriptor.entryPoint;
		object.pSpecializationInfo = nullptr;
	}

	VulkanShaderModule::~VulkanShaderModule() {
		PROFILE_SCOPE(FUNC_SIG);

		vkDestroyShaderModule(device, object.module, nullptr);
	}
}