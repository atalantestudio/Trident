// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/VK/DescriptorSetLayout.hpp"
#include "Engine/Adapter/Graphics/VK/DescriptorType.hpp"
#include "Engine/Adapter/Graphics/VK/ShaderStage.hpp"

namespace ProjectA {
	VK::DescriptorSetLayout::DescriptorSetLayout(const sequence<DescriptorSetLayoutBinding>& descriptorSetLayoutBindings, const VK::Device& device) :
		ProjectA::DescriptorSetLayout(descriptorSetLayoutBindings),
		device(device)
	{
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		sequence<VkDescriptorSetLayoutBinding> vkDescriptorSetLayoutBindings(descriptorSetLayoutBindings.count());

		VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo{};
		descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		descriptorSetLayoutCreateInfo.pNext = nullptr;
		descriptorSetLayoutCreateInfo.flags = 0;
		descriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32>(vkDescriptorSetLayoutBindings.count());

		for (uint32 descriptorSetLayoutBindingIndex = 0; descriptorSetLayoutBindingIndex < descriptorSetLayoutBindings.count(); descriptorSetLayoutBindingIndex += 1) {
			const DescriptorSetLayoutBinding& descriptorSetLayoutBinding = descriptorSetLayoutBindings[descriptorSetLayoutBindingIndex];

			VkDescriptorSetLayoutBinding& vkDescriptorSetLayoutBinding = vkDescriptorSetLayoutBindings[descriptorSetLayoutBindingIndex];
			vkDescriptorSetLayoutBinding.binding = descriptorSetLayoutBindingIndex;
			vkDescriptorSetLayoutBinding.descriptorType = getDescriptorType(descriptorSetLayoutBinding.descriptorType);
			vkDescriptorSetLayoutBinding.descriptorCount = descriptorSetLayoutBinding.descriptorCount;
			vkDescriptorSetLayoutBinding.stageFlags = getShaderStageFlagBits(descriptorSetLayoutBinding.shaderStages);
			vkDescriptorSetLayoutBinding.pImmutableSamplers = nullptr;
		}

		descriptorSetLayoutCreateInfo.pBindings = &vkDescriptorSetLayoutBindings[0];

		const VkResult result = vkCreateDescriptorSetLayout(device.device, &descriptorSetLayoutCreateInfo, nullptr, &descriptorSetLayout);

		ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to create descriptor set layout ([]).", string_VkResult(result));
	}

	VK::DescriptorSetLayout::~DescriptorSetLayout() {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		vkDestroyDescriptorSetLayout(device.device, descriptorSetLayout, nullptr);
	}
}