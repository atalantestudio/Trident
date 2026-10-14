// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#if defined(DEBUG_OBJECT_NAME)
	#include "Engine/Adapter/Graphics/VK/Debug/ObjectName.hpp"
#endif

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

		ref<VK::PipelineLayout> pipelineLayout = allocateRef<VK::PipelineLayout>(device);

		sequence<VkDescriptorSetLayout> vkDescriptorSetLayouts(descriptor.descriptorSetLayouts.count());
		uint32 descriptorSetLayoutIndex = 0;

		while (descriptorSetLayoutIndex < descriptor.descriptorSetLayouts.count()) {
			vkDescriptorSetLayouts[descriptorSetLayoutIndex] = ref_cast<VK::DescriptorSetLayout>(descriptor.descriptorSetLayouts[descriptorSetLayoutIndex])->descriptorSetLayout;

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

		ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to create pipeline layout ([]).", static_cast<sint32>(result));

		#if defined(DEBUG_OBJECT_NAME)
			setObjectName(_device, (uint64) pipelineLayout->pipelineLayout, VK_OBJECT_TYPE_PIPELINE_LAYOUT, descriptor.name);
		#endif

		return pipelineLayout;
	}

	void VK::destroyPipelineLayout(const Trident::Device* device, const Trident::PipelineLayout* pipelineLayout) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const VK::Device* _device = static_cast<const VK::Device*>(device);
		const VK::PipelineLayout* _pipelineLayout = static_cast<const VK::PipelineLayout*>(pipelineLayout);

		_device->vkDestroyPipelineLayout(_device->device, _pipelineLayout->pipelineLayout, nullptr);
	}
}