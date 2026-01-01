#include "pch.hpp"

#include "Backend/VK/DescriptorSetLayout.hpp"
#include "Backend/VK/PipelineLayout.hpp"

namespace ProjectA {
	VulkanPipelineLayout::VulkanPipelineLayout(VkDevice device, const PipelineLayoutDescriptor& descriptor) :
		PipelineLayout(descriptor),
		device(device)
	{
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		sequence<VkDescriptorSetLayout> vkDescriptorSetLayouts(descriptor.descriptorSetLayouts.count());

		for (uint32 descriptorSetLayoutIndex = 0; descriptorSetLayoutIndex < descriptor.descriptorSetLayouts.count(); descriptorSetLayoutIndex += 1) {
			vkDescriptorSetLayouts[descriptorSetLayoutIndex] = std::static_pointer_cast<VulkanDescriptorSetLayout>(descriptor.descriptorSetLayouts[descriptorSetLayoutIndex])->object;
		}

		VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
		pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutCreateInfo.pNext = nullptr;
		pipelineLayoutCreateInfo.flags = 0;
		pipelineLayoutCreateInfo.setLayoutCount = static_cast<uint32>(vkDescriptorSetLayouts.count());
		pipelineLayoutCreateInfo.pSetLayouts = &vkDescriptorSetLayouts[0];
		pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
		pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;

		const VkResult result = vkCreatePipelineLayout(device, &pipelineLayoutCreateInfo, nullptr, &object);

		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to create Vulkan pipeline layout ([]).", string_VkResult(result));
	}

	VulkanPipelineLayout::~VulkanPipelineLayout() {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		// NOTE: Only if the pool has been created with the VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT flag.
		// const VkResult result = vkFreeDescriptorSets(device, descriptorPool, static_cast<uint32>(descriptorSets.size()), descriptorSets.data());
		// ENGINE_ASSERT(result == VK_SUCCESS, "Failed to free Vulkan descriptor sets ([]).", string_VkResult(result));

		vkDestroyPipelineLayout(device, object, nullptr);
	}
}