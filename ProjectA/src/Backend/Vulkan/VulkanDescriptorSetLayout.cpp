#include "pch.hpp"

#include "Backend/Vulkan/VulkanDescriptorSetLayout.hpp"

#include "Backend/Vulkan/DescriptorType.hpp"
#include "Backend/Vulkan/ShaderStage.hpp"

namespace ProjectA {
	VulkanDescriptorSetLayout::VulkanDescriptorSetLayout(VkDevice device, const sequence<DescriptorSetLayoutBinding>& descriptorSetLayoutBindings) :
		DescriptorSetLayout(descriptorSetLayoutBindings),
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
			vkDescriptorSetLayoutBinding.descriptorType = getVkDescriptorType(descriptorSetLayoutBinding.descriptorType);
			vkDescriptorSetLayoutBinding.descriptorCount = descriptorSetLayoutBinding.descriptorCount;
			vkDescriptorSetLayoutBinding.stageFlags = getVkShaderStageFlagBits(descriptorSetLayoutBinding.shaderStages);
			vkDescriptorSetLayoutBinding.pImmutableSamplers = nullptr;
		}

		descriptorSetLayoutCreateInfo.pBindings = &vkDescriptorSetLayoutBindings[0];

		const VkResult result = vkCreateDescriptorSetLayout(device, &descriptorSetLayoutCreateInfo, nullptr, &object);

		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to create Vulkan descriptor set layout ([]).", string_VkResult(result));
	}

	VulkanDescriptorSetLayout::~VulkanDescriptorSetLayout() {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		vkDestroyDescriptorSetLayout(device, object, nullptr);
	}
}