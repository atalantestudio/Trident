// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#if defined(DEBUG_OBJECT_NAME)
	#include "Engine/Adapter/Graphics/VK/Debug/ObjectName.hpp"
#endif

#include "Engine/Adapter/Graphics/VK/ShaderModule.hpp"
#include "Engine/Adapter/Graphics/VK/ShaderStage.hpp"
#include "Engine/API/Graphics/Descriptor/ShaderModuleDescriptor.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"
#include "Engine/Filesystem/Asset/ShaderModuleAsset.hpp"

namespace Trident {
	VK::ShaderModule::ShaderModule(const Trident::Device* device) :
		Trident::ShaderModule(device)
	{}

	ref<Trident::ShaderModule> VK::createShaderModule(const Trident::Device* device, const ShaderModuleDescriptor& descriptor, ref<ShaderModuleAsset> shaderModuleAsset) {
		ADAPTER_ASSERT(false, "Shader module assets are not supported by the Vulkan adapter.");

		return nullptr;
	}

	ref<Trident::ShaderModule> VK::createShaderModule(const Trident::Device* device, const ShaderModuleDescriptor& descriptor, view<char8> code) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const VK::Device* _device = static_cast<const VK::Device*>(device);
		VkResult result;

		ref<VK::ShaderModule> shaderModule = allocateRef<VK::ShaderModule>(device);

		VkShaderModuleCreateInfo shaderModuleCreateInfo{};
		shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		shaderModuleCreateInfo.pNext = nullptr;
		shaderModuleCreateInfo.flags = 0;
		shaderModuleCreateInfo.codeSize = code.count();
		shaderModuleCreateInfo.pCode = pointer_cast<const uint32*>(&code[0]);

		result = _device->vkCreateShaderModule(_device->device, &shaderModuleCreateInfo, nullptr, &shaderModule->createInfo.module);

		ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to create shader module ([]).", string_VkResult(result));

		#if defined(DEBUG_OBJECT_NAME)
			setObjectName(_device, (uint64) shaderModule->createInfo.module, VK_OBJECT_TYPE_SHADER_MODULE, descriptor.name);
		#endif

		shaderModule->createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shaderModule->createInfo.pNext = nullptr;
		shaderModule->createInfo.flags = 0;
		shaderModule->createInfo.stage = getShaderStageFlagBits(descriptor.stage);
		shaderModule->createInfo.pName = &descriptor.entryPoint[0];
		shaderModule->createInfo.pSpecializationInfo = nullptr;

		return shaderModule;
	}

	void VK::destroyShaderModule(const Trident::Device* device, const Trident::ShaderModule* shaderModule) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const VK::Device* _device = static_cast<const VK::Device*>(device);
		const VK::ShaderModule* _shaderModule = static_cast<const VK::ShaderModule*>(shaderModule);

		_device->vkDestroyShaderModule(_device->device, _shaderModule->createInfo.module, nullptr);
	}
}