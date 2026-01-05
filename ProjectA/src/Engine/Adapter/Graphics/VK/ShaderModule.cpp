#include "pch.hpp"

#include "Engine/Adapter/Graphics/VK/ShaderModule.hpp"
#include "Engine/Adapter/Graphics/VK/ShaderStage.hpp"

namespace ProjectA {
	VulkanShaderModule::VulkanShaderModule(VkDevice device, const ref<ShaderModuleAsset>& asset, const ShaderModuleDescriptor& descriptor) :
		ShaderModule(descriptor),
		device(device)
	{
		// TODO
		ENGINE_ERROR_UNIMPLEMENTED();
	}

	VulkanShaderModule::VulkanShaderModule(view<char8> code, const ShaderModuleDescriptor& descriptor, VkDevice device) :
		ShaderModule(descriptor),
		device(device)
	{
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		VkShaderModuleCreateInfo shaderModuleCreateInfo{};
		shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		shaderModuleCreateInfo.pNext = nullptr;
		shaderModuleCreateInfo.flags = 0;
		shaderModuleCreateInfo.codeSize = code.count();
		shaderModuleCreateInfo.pCode = pointer_cast<const uint32*>(&code[0]);

		const VkResult result = vkCreateShaderModule(device, &shaderModuleCreateInfo, nullptr, &object.module);

		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to create Vulkan shader module ([]).", string_VkResult(result));

		object.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		object.pNext = nullptr;
		object.flags = 0;
		object.stage = getVkShaderStageFlagBits(descriptor.stage);
		object.pName = &descriptor.entryPoint[0];
		object.pSpecializationInfo = nullptr;
	}

	VulkanShaderModule::~VulkanShaderModule() {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		vkDestroyShaderModule(device, object.module, nullptr);
	}
}