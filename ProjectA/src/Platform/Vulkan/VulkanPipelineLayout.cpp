#include "pch.hpp"

#include "Platform/Vulkan/DescriptorType.hpp"
#include "Platform/Vulkan/ShaderStage.hpp"
#include "Platform/Vulkan/VulkanPipelineLayout.hpp"

namespace ProjectA {
	VulkanPipelineLayout::VulkanPipelineLayout(const VkDevice device, const PipelineLayoutDescriptor& descriptor) :
		device(device)
	{
		PROFILE_SCOPE(FUNC_SIG);

		ENGINE_TRACE("[~VulkanPipelineLayout]");

		std::vector<VkDescriptorSetLayout> descriptorSetLayouts;

		for (const DescriptorSetLayout& layout : descriptor.layouts) {
			std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings;

			for (uint32 i = 0; i < layout.bindings.size(); i++) {
				const DescriptorSetLayoutBinding& binding = layout.bindings[i];

				VkDescriptorSetLayoutBinding descriptorSetLayoutBinding{};
				descriptorSetLayoutBinding.binding = i;
				descriptorSetLayoutBinding.descriptorType = getVkDescriptorType(binding.descriptorType);
				descriptorSetLayoutBinding.descriptorCount = binding.descriptorCount;
				descriptorSetLayoutBinding.stageFlags = getVkShaderStageFlagBits(binding.stageFlags);
				descriptorSetLayoutBinding.pImmutableSamplers = nullptr;

				descriptorSetLayoutBindings.emplace_back(descriptorSetLayoutBinding);
			}

			VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo{};
			descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			descriptorSetLayoutCreateInfo.pNext = nullptr;
			descriptorSetLayoutCreateInfo.flags = 0;
			descriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32>(descriptorSetLayoutBindings.size());
			descriptorSetLayoutCreateInfo.pBindings = descriptorSetLayoutBindings.data();

			VkDescriptorSetLayout descriptorSetLayout;

			if (vkCreateDescriptorSetLayout(device, &descriptorSetLayoutCreateInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
				throw std::runtime_error("Failed to create Vulkan descriptor set layout.");
			}

			descriptorSetLayouts.emplace_back(descriptorSetLayout);
		}

		VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
		pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutCreateInfo.pNext = nullptr;
		pipelineLayoutCreateInfo.flags = 0;
		pipelineLayoutCreateInfo.setLayoutCount = static_cast<uint32>(descriptorSetLayouts.size());
		pipelineLayoutCreateInfo.pSetLayouts = descriptorSetLayouts.data();
		pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
		pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;

		if (vkCreatePipelineLayout(device, &pipelineLayoutCreateInfo, nullptr, &object) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create Vulkan pipeline layout.");
		}

		for (const VkDescriptorSetLayout descriptorSetLayout : descriptorSetLayouts) {
			vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);
		}
	}

	VulkanPipelineLayout::~VulkanPipelineLayout() {
		PROFILE_SCOPE(FUNC_SIG);

		ENGINE_TRACE("[~VulkanPipelineLayout]");

		vkDestroyPipelineLayout(device, object, nullptr);
	}
}