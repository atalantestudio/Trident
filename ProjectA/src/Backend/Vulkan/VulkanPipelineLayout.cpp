#include "pch.hpp"

#include "Backend/Vulkan/VulkanBuffer.hpp"
#include "Backend/Vulkan/VulkanImageView.hpp"
#include "Backend/Vulkan/VulkanPipelineLayout.hpp"
#include "Backend/Vulkan/VulkanSampler.hpp"

#include "Backend/Vulkan/DescriptorType.hpp"
#include "Backend/Vulkan/ShaderStage.hpp"

namespace ProjectA {
	VulkanPipelineLayout::VulkanPipelineLayout(VkDevice device, const PipelineLayoutDescriptor& descriptor) :
		PipelineLayout(descriptor),
		device(device)
	{
		PROFILE_SCOPE(FUNC_SIG);

		std::vector<VkDescriptorPoolSize> descriptorPoolSizes;
		VkResult result;

		for (const DescriptorSetLayout& layout : descriptor.layouts) {
			std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings;

			for (uint32 bindingIndex = 0; bindingIndex < layout.bindings.size(); bindingIndex += 1) {
				const DescriptorSetLayoutBinding& binding = layout.bindings[bindingIndex];
				const VkDescriptorType descriptorType = getVkDescriptorType(binding.descriptorType);

				// TODO(review): lyah::max
				const uint32 descriptorCount = static_cast<uint32>(std::max(binding.buffers.size(), binding.imageViews.size()));

				VkDescriptorSetLayoutBinding descriptorSetLayoutBinding{};
				descriptorSetLayoutBinding.binding = bindingIndex;
				descriptorSetLayoutBinding.descriptorType = descriptorType;
				descriptorSetLayoutBinding.descriptorCount = descriptorCount;
				descriptorSetLayoutBinding.stageFlags = getVkShaderStageFlagBits(binding.stageFlags);
				descriptorSetLayoutBinding.pImmutableSamplers = nullptr;

				descriptorSetLayoutBindings.emplace_back(descriptorSetLayoutBinding);

				VkDescriptorPoolSize descriptorPoolSize{};
				descriptorPoolSize.type = descriptorType;

				if (
					binding.descriptorType == DescriptorType::COMBINED_IMAGE_SAMPLER ||
					binding.descriptorType == DescriptorType::STORAGE_IMAGE
				) {
					descriptorPoolSize.descriptorCount = static_cast<uint32>(binding.imageViews.size());
				}
				else {
					descriptorPoolSize.descriptorCount = static_cast<uint32>(binding.buffers.size());
				}

				descriptorPoolSizes.emplace_back(descriptorPoolSize);
			}

			VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo{};
			descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			descriptorSetLayoutCreateInfo.pNext = nullptr;
			descriptorSetLayoutCreateInfo.flags = 0;
			descriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32>(descriptorSetLayoutBindings.size());
			descriptorSetLayoutCreateInfo.pBindings = descriptorSetLayoutBindings.data();

			VkDescriptorSetLayout descriptorSetLayout;

			result = vkCreateDescriptorSetLayout(device, &descriptorSetLayoutCreateInfo, nullptr, &descriptorSetLayout);

			ENGINE_ASSERT(result == VK_SUCCESS, "Failed to create Vulkan descriptor set layout ([]).", string_VkResult(result));

			descriptorSetLayouts.emplace_back(descriptorSetLayout);
		}

		if (descriptorSetLayouts.size() > 0) {
			VkDescriptorPoolCreateInfo descriptorPoolCreateInfo{};
			descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			descriptorPoolCreateInfo.pNext = nullptr;
			descriptorPoolCreateInfo.flags = 0;
			descriptorPoolCreateInfo.maxSets = static_cast<uint32>(descriptorSetLayouts.size());
			descriptorPoolCreateInfo.poolSizeCount = static_cast<uint32>(descriptorPoolSizes.size());
			descriptorPoolCreateInfo.pPoolSizes = descriptorPoolSizes.data();

			result = vkCreateDescriptorPool(device, &descriptorPoolCreateInfo, nullptr, &descriptorPool);

			ENGINE_ASSERT(result == VK_SUCCESS, "Failed to create Vulkan descriptor pool ([]).", string_VkResult(result));

			VkDescriptorSetAllocateInfo descriptorSetAllocateInfo{};
			descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			descriptorSetAllocateInfo.pNext = nullptr;
			descriptorSetAllocateInfo.descriptorPool = descriptorPool;
			descriptorSetAllocateInfo.descriptorSetCount = static_cast<uint32>(descriptorSetLayouts.size());
			descriptorSetAllocateInfo.pSetLayouts = descriptorSetLayouts.data();

			descriptorSets.resize(descriptorSetLayouts.size());

			result = vkAllocateDescriptorSets(device, &descriptorSetAllocateInfo, descriptorSets.data());

			ENGINE_ASSERT(result == VK_SUCCESS, "Failed to allocate Vulkan descriptor sets ([]).", string_VkResult(result));

			std::vector<VkDescriptorBufferInfo> descriptorBufferInfos;
			std::vector<VkDescriptorImageInfo> descriptorImageInfos;

			for (const DescriptorSetLayout& layout : descriptor.layouts) {
				for (const DescriptorSetLayoutBinding& binding : layout.bindings) {
					for (const ref<Buffer>& buffer : binding.buffers) {
						const VulkanBuffer& vkBuffer = *std::static_pointer_cast<VulkanBuffer>(buffer);

						VkDescriptorBufferInfo descriptorBufferInfo{};
						descriptorBufferInfo.buffer = vkBuffer.object;
						descriptorBufferInfo.offset = binding.bufferOffset;
						descriptorBufferInfo.range = vkBuffer.size - binding.bufferOffset;

						descriptorBufferInfos.emplace_back(descriptorBufferInfo);
					}

					for (uint32 imageViewIndex = 0; imageViewIndex < binding.imageViews.size(); imageViewIndex += 1) {
						const ref<VulkanImageView> imageView = std::static_pointer_cast<VulkanImageView>(binding.imageViews[imageViewIndex]);

						VkSampler sampler = VK_NULL_HANDLE;

						if (imageViewIndex < binding.samplers.size()) {
							sampler = std::static_pointer_cast<VulkanSampler>(binding.samplers[imageViewIndex])->object;
						}

						VkDescriptorImageInfo descriptorImageInfo{};
						descriptorImageInfo.sampler = sampler;
						descriptorImageInfo.imageView = imageView->object;

						// TODO: Specify the current image's layout.
						if (binding.descriptorType == DescriptorType::COMBINED_IMAGE_SAMPLER) {
							descriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
						}
						else if (binding.descriptorType == DescriptorType::STORAGE_IMAGE) {
							// Can also be VK_IMAGE_LAYOUT_SHARED_PRESENT_KHR.
							descriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
						}

						descriptorImageInfos.emplace_back(descriptorImageInfo);
					}
				}
			}

			std::vector<VkWriteDescriptorSet> writeDescriptorSets;

			uint32 bufferWriteDescriptorSetIndex = 0;
			uint32 imageWriteDescriptorSetIndex = 0;

			for (uint32 layoutIndex = 0; layoutIndex < descriptor.layouts.size(); layoutIndex += 1) {
				const DescriptorSetLayout& layout = descriptor.layouts[layoutIndex];
				const VkDescriptorSet descriptorSet = descriptorSets[layoutIndex];

				for (uint32 bindingIndex = 0; bindingIndex < layout.bindings.size(); bindingIndex += 1) {
					const DescriptorSetLayoutBinding& binding = layout.bindings[bindingIndex];

					// Create 1 write descriptor set per binding.
					VkWriteDescriptorSet writeDescriptorSet{};
					writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
					writeDescriptorSet.pNext = nullptr;
					writeDescriptorSet.dstSet = descriptorSet;
					writeDescriptorSet.dstBinding = bindingIndex;
					writeDescriptorSet.dstArrayElement = 0;

					if (!binding.buffers.empty()) {
						writeDescriptorSet.descriptorCount = static_cast<uint32>(binding.buffers.size());
					}
					else if (!binding.imageViews.empty()) {
						writeDescriptorSet.descriptorCount = static_cast<uint32>(binding.imageViews.size());
					}

					writeDescriptorSet.descriptorType = getVkDescriptorType(binding.descriptorType);

					if (!binding.buffers.empty()) {
						writeDescriptorSet.pImageInfo = nullptr;
						writeDescriptorSet.pBufferInfo = &descriptorBufferInfos[bufferWriteDescriptorSetIndex];

						bufferWriteDescriptorSetIndex += 1;
					}
					else if (!binding.imageViews.empty()) {
						writeDescriptorSet.pImageInfo = &descriptorImageInfos[imageWriteDescriptorSetIndex];
						writeDescriptorSet.pBufferInfo = nullptr;

						imageWriteDescriptorSetIndex += 1;
					}

					writeDescriptorSet.pTexelBufferView = nullptr;

					writeDescriptorSets.emplace_back(writeDescriptorSet);
				}
			}

			vkUpdateDescriptorSets(device, static_cast<uint32>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, nullptr);
		}

		VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
		pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutCreateInfo.pNext = nullptr;
		pipelineLayoutCreateInfo.flags = 0;
		pipelineLayoutCreateInfo.setLayoutCount = static_cast<uint32>(descriptorSetLayouts.size());
		pipelineLayoutCreateInfo.pSetLayouts = descriptorSetLayouts.data();
		pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
		pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;

		result = vkCreatePipelineLayout(device, &pipelineLayoutCreateInfo, nullptr, &object);

		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to create Vulkan pipeline layout ([]).", string_VkResult(result));
	}

	VulkanPipelineLayout::~VulkanPipelineLayout() {
		PROFILE_SCOPE(FUNC_SIG);

		if (descriptorSetLayouts.size() > 0) {
			// NOTE: Only if the pool has been created with the VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT flag.
			/*const VkResult result = vkFreeDescriptorSets(device, descriptorPool, static_cast<uint32>(descriptorSets.size()), descriptorSets.data());

			ENGINE_ASSERT(result == VK_SUCCESS, "Failed to free Vulkan descriptor sets ([]).", string_VkResult(result));*/

			vkDestroyDescriptorPool(device, descriptorPool, nullptr);

			for (const VkDescriptorSetLayout descriptorSetLayout : descriptorSetLayouts) {
				vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);
			}
		}

		vkDestroyPipelineLayout(device, object, nullptr);
	}
}