// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/VK/DescriptorSetLayout.hpp"
#include "Engine/Adapter/Graphics/VK/PipelineLayout.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace Trident {
	VK::PipelineLayout::PipelineLayout(const Trident::Device* device) :
		Trident::PipelineLayout(device)
	{}

	ref<Trident::PipelineLayout> VK::createPipelineLayout(const Trident::Device* device, const PipelineLayoutDescriptor& descriptor) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const VK::Device* _device = static_cast<const VK::Device*>(device);
		VkResult result;

		ref<VK::PipelineLayout> pipelineLayout = createRef<VK::PipelineLayout>(device);

		sequence<VkDescriptorSetLayout> vkDescriptorSetLayouts(descriptor.descriptorSetLayouts.count());
		uint32 descriptorSetLayoutIndex = 0;

		while (descriptorSetLayoutIndex < descriptor.descriptorSetLayouts.count()) {
			vkDescriptorSetLayouts[descriptorSetLayoutIndex] = std::static_pointer_cast<VK::DescriptorSetLayout>(descriptor.descriptorSetLayouts[descriptorSetLayoutIndex])->descriptorSetLayout;

			descriptorSetLayoutIndex += 1;
		}

		VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
		pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutCreateInfo.pNext = nullptr;
		pipelineLayoutCreateInfo.flags = 0;
		pipelineLayoutCreateInfo.setLayoutCount = static_cast<uint32>(vkDescriptorSetLayouts.count());
		pipelineLayoutCreateInfo.pSetLayouts = &vkDescriptorSetLayouts[0];
		pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
		pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;

		result = _device->vkCreatePipelineLayout(_device->device, &pipelineLayoutCreateInfo, nullptr, &pipelineLayout->pipelineLayout);

		ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to create pipeline layout ([]).", string_VkResult(result));

		#if defined(BUILD_MODE_DEBUG)
			if (descriptor.name.count() > 0) {
				VkDebugUtilsObjectNameInfoEXT debugUtilsObjectNameInfo{};
				debugUtilsObjectNameInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
				debugUtilsObjectNameInfo.pNext = nullptr;
				debugUtilsObjectNameInfo.objectType = VK_OBJECT_TYPE_PIPELINE_LAYOUT;
				debugUtilsObjectNameInfo.objectHandle = (uint64) pipelineLayout->pipelineLayout;
				debugUtilsObjectNameInfo.pObjectName = &descriptor.name[0];

				result = _device->vkSetDebugUtilsObjectNameEXT(_device->device, &debugUtilsObjectNameInfo);

				ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to set object name ([]).", string_VkResult(result));
			}
		#endif

		return pipelineLayout;
	}

	void VK::destroyPipelineLayout(const Trident::Device* device, const Trident::PipelineLayout* pipelineLayout) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const VK::Device* _device = static_cast<const VK::Device*>(device);
		const VK::PipelineLayout* _pipelineLayout = static_cast<const VK::PipelineLayout*>(pipelineLayout);

		// TODO: Only if the pool has been created with the VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT flag.
		/* VkResult result;

		result = vkFreeDescriptorSets(_device->device, descriptorPool, static_cast<uint32>(descriptorSets.count()), &descriptorSets[0]);

		ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to free descriptor sets ([]).", string_VkResult(result)); */

		_device->vkDestroyPipelineLayout(_device->device, _pipelineLayout->pipelineLayout, nullptr);
	}
}