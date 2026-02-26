// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/VK/DescriptorSetLayout.hpp"
#include "Engine/Adapter/Graphics/VK/DescriptorType.hpp"
#include "Engine/Adapter/Graphics/VK/Device.hpp"
#include "Engine/Adapter/Graphics/VK/ShaderStage.hpp"

namespace Trident {
	VK::DescriptorSetLayout::DescriptorSetLayout(const Trident::Device* device, const sequence<DescriptorSetLayoutBinding>& bindings) :
		Trident::DescriptorSetLayout(device, bindings)
	{}

	ref<Trident::DescriptorSetLayout> VK::createDescriptorSetLayout(const Trident::Device* device, const sequence<DescriptorSetLayoutBinding>& bindings) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const VK::Device* _device = static_cast<const VK::Device*>(device);
		sequence<VkDescriptorSetLayoutBinding> vkDescriptorSetLayoutBindings(bindings.count());
		ref<VK::DescriptorSetLayout> descriptorSetLayout = createRef<VK::DescriptorSetLayout>(device, bindings);
		VkResult result;

		VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo{};
		descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		descriptorSetLayoutCreateInfo.pNext = nullptr;
		descriptorSetLayoutCreateInfo.flags = 0;
		descriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32>(vkDescriptorSetLayoutBindings.count());

		uint32 bindingIndex = 0;

		while (bindingIndex < bindings.count()) {
			const DescriptorSetLayoutBinding& binding = bindings[bindingIndex];

			VkDescriptorSetLayoutBinding& vkDescriptorSetLayoutBinding = vkDescriptorSetLayoutBindings[bindingIndex];
			vkDescriptorSetLayoutBinding.binding = bindingIndex;
			vkDescriptorSetLayoutBinding.descriptorType = getDescriptorType(binding.descriptorType);
			vkDescriptorSetLayoutBinding.descriptorCount = binding.descriptorCount;
			vkDescriptorSetLayoutBinding.stageFlags = getShaderStageFlagBits(binding.shaderStages);
			vkDescriptorSetLayoutBinding.pImmutableSamplers = nullptr;

			bindingIndex += 1;
		}

		descriptorSetLayoutCreateInfo.pBindings = &vkDescriptorSetLayoutBindings[0];

		result = vkCreateDescriptorSetLayout(_device->device, &descriptorSetLayoutCreateInfo, nullptr, &descriptorSetLayout->descriptorSetLayout);

		ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to create descriptor set layout ([]).", string_VkResult(result));

		return descriptorSetLayout;
	}

	void VK::destroyDescriptorSetLayout(const Trident::Device* device, const Trident::DescriptorSetLayout* descriptorSetLayout) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const VK::Device* _device = static_cast<const VK::Device*>(device);
		const VK::DescriptorSetLayout* _descriptorSetLayout = static_cast<const VK::DescriptorSetLayout*>(descriptorSetLayout);

		_device->vkDestroyDescriptorSetLayout(_device->device, _descriptorSetLayout->descriptorSetLayout, nullptr);
	}
}