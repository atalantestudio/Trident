// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/VK/DescriptorSetLayout.hpp"
#include "Engine/Adapter/Graphics/VK/DescriptorType.hpp"
#include "Engine/Adapter/Graphics/VK/Device.hpp"
#include "Engine/Adapter/Graphics/VK/ShaderStage.hpp"

namespace Trident {
	VK::DescriptorSetLayout::DescriptorSetLayout(const Trident::Device* device, const DescriptorSetLayoutDescriptor& descriptor) :
		Trident::DescriptorSetLayout(device, descriptor)
	{}

	ref<Trident::DescriptorSetLayout> VK::createDescriptorSetLayout(const Trident::Device* device, const DescriptorSetLayoutDescriptor& descriptor) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const VK::Device* _device = static_cast<const VK::Device*>(device);

		ref<VK::DescriptorSetLayout> descriptorSetLayout = allocateRef<VK::DescriptorSetLayout>(device, descriptor);
		sequence<VkDescriptorSetLayoutBinding> vkDescriptorSetLayoutBindings(descriptor.bindings.count());
		VkResult result;

		VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo{};
		descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		descriptorSetLayoutCreateInfo.pNext = nullptr;
		descriptorSetLayoutCreateInfo.flags = 0;
		descriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32>(vkDescriptorSetLayoutBindings.count());

		uint32 bindingIndex = 0;

		while (bindingIndex < descriptor.bindings.count()) {
			const DescriptorSetLayoutBinding& binding = descriptor.bindings[bindingIndex];

			VkDescriptorSetLayoutBinding& vkDescriptorSetLayoutBinding = vkDescriptorSetLayoutBindings[bindingIndex];
			vkDescriptorSetLayoutBinding.binding = bindingIndex;
			vkDescriptorSetLayoutBinding.descriptorType = getDescriptorType(binding.descriptorType);
			vkDescriptorSetLayoutBinding.descriptorCount = binding.descriptorCount;
			vkDescriptorSetLayoutBinding.stageFlags = getShaderStageFlagBits(binding.stageFlags);
			vkDescriptorSetLayoutBinding.pImmutableSamplers = nullptr;

			bindingIndex += 1;
		}

		descriptorSetLayoutCreateInfo.pBindings = &vkDescriptorSetLayoutBindings[0];

		result = _device->vkCreateDescriptorSetLayout(_device->device, &descriptorSetLayoutCreateInfo, nullptr, &descriptorSetLayout->descriptorSetLayout);

		ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to create descriptor set layout ([]).", string_VkResult(result));

		#if defined(DEBUG_OBJECT_NAME)
			if (descriptor.name.count() > 0) {
				VkDebugUtilsObjectNameInfoEXT debugUtilsObjectNameInfo{};
				debugUtilsObjectNameInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
				debugUtilsObjectNameInfo.pNext = nullptr;
				debugUtilsObjectNameInfo.objectType = VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT;
				debugUtilsObjectNameInfo.objectHandle = (uint64) descriptorSetLayout->descriptorSetLayout;
				debugUtilsObjectNameInfo.pObjectName = &descriptor.name[0];

				result = _device->vkSetDebugUtilsObjectNameEXT(_device->device, &debugUtilsObjectNameInfo);

				ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to set object name ([]).", string_VkResult(result));
			}
		#endif

		return descriptorSetLayout;
	}

	void VK::destroyDescriptorSetLayout(const Trident::Device* device, const Trident::DescriptorSetLayout* descriptorSetLayout) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const VK::Device* _device = static_cast<const VK::Device*>(device);
		const VK::DescriptorSetLayout* _descriptorSetLayout = static_cast<const VK::DescriptorSetLayout*>(descriptorSetLayout);

		_device->vkDestroyDescriptorSetLayout(_device->device, _descriptorSetLayout->descriptorSetLayout, nullptr);
	}
}