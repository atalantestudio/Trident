// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/VK/ShaderModule.hpp"
#include "Engine/Adapter/Graphics/VK/ShaderStage.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace ProjectA {
	VK::ShaderModule::ShaderModule(const ShaderModuleDescriptor& descriptor, view<char8> code, const VK::Device& device) :
		ProjectA::ShaderModule(descriptor),
		device(device)
	{
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		VkShaderModuleCreateInfo shaderModuleCreateInfo{};
		shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		shaderModuleCreateInfo.pNext = nullptr;
		shaderModuleCreateInfo.flags = 0;
		shaderModuleCreateInfo.codeSize = code.count();
		shaderModuleCreateInfo.pCode = pointer_cast<const uint32*>(&code[0]);

		const VkResult result = vkCreateShaderModule(device.device, &shaderModuleCreateInfo, nullptr, &createInfo.module);

		ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to create shader module ([]).", string_VkResult(result));

		createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		createInfo.pNext = nullptr;
		createInfo.flags = 0;
		createInfo.stage = getShaderStageFlagBits(descriptor.stage);
		createInfo.pName = &descriptor.entryPoint[0];
		createInfo.pSpecializationInfo = nullptr;
	}

	VK::ShaderModule::~ShaderModule() {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		vkDestroyShaderModule(device.device, createInfo.module, nullptr);
	}
}