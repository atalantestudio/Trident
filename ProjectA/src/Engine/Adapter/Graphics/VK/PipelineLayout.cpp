// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/VK/DescriptorSetLayout.hpp"
#include "Engine/Adapter/Graphics/VK/PipelineLayout.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace ProjectA {
	VK::PipelineLayout::PipelineLayout(const PipelineLayoutDescriptor& descriptor, const VK::Device& device) :
		ProjectA::PipelineLayout(descriptor),
		device(device)
	{
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		sequence<VkDescriptorSetLayout> vkDescriptorSetLayouts(descriptor.descriptorSetLayouts.count());

		for (uint32 descriptorSetLayoutIndex = 0; descriptorSetLayoutIndex < descriptor.descriptorSetLayouts.count(); descriptorSetLayoutIndex += 1) {
			vkDescriptorSetLayouts[descriptorSetLayoutIndex] = std::static_pointer_cast<VK::DescriptorSetLayout>(descriptor.descriptorSetLayouts[descriptorSetLayoutIndex])->descriptorSetLayout;
		}

		VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
		pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutCreateInfo.pNext = nullptr;
		pipelineLayoutCreateInfo.flags = 0;
		pipelineLayoutCreateInfo.setLayoutCount = static_cast<uint32>(vkDescriptorSetLayouts.count());
		pipelineLayoutCreateInfo.pSetLayouts = &vkDescriptorSetLayouts[0];
		pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
		pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;

		const VkResult result = vkCreatePipelineLayout(device.device, &pipelineLayoutCreateInfo, nullptr, &pipelineLayout);

		ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to create pipeline layout ([]).", string_VkResult(result));
	}

	VK::PipelineLayout::~PipelineLayout() {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		// TODO: Only if the pool has been created with the VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT flag.
		// const VkResult result = vkFreeDescriptorSets(device, descriptorPool, static_cast<uint32>(descriptorSets.size()), descriptorSets.data());
		// ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to free descriptor sets ([]).", string_VkResult(result));

		vkDestroyPipelineLayout(device.device, pipelineLayout, nullptr);
	}
}