#include "pch.hpp"

#include "Backend/Vulkan/DescriptorType.hpp"
#include "Backend/Vulkan/ShaderStage.hpp"
#include "Backend/Vulkan/VulkanBuffer.hpp"
#include "Backend/Vulkan/VulkanImageView.hpp"
#include "Backend/Vulkan/VulkanPipelineLayout.hpp"
#include "Backend/Vulkan/VulkanSampler.hpp"

namespace ProjectA {
	VulkanPipelineLayout::VulkanPipelineLayout(VkDevice device, const PipelineLayoutDescriptor& descriptor) :
		device(device)
	{
		PROFILE_SCOPE(FUNC_SIG);

		chain<VkDescriptorPoolSize> descriptorPoolSizes;
		VkResult result;

		for (const DescriptorSetLayout& layout : descriptor.layouts) {
			chain<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings;

			for (uint32 bindingIndex = 0; bindingIndex < layout.bindings.size(); bindingIndex += 1) {
				const DescriptorSetLayoutBinding& binding = layout.bindings[bindingIndex];
				const VkDescriptorType descriptorType = getVkDescriptorType(binding.descriptorType);

				VkDescriptorSetLayoutBinding descriptorSetLayoutBinding{};
				descriptorSetLayoutBinding.binding = bindingIndex;
				descriptorSetLayoutBinding.descriptorType = descriptorType;
				descriptorSetLayoutBinding.descriptorCount = 1;
				descriptorSetLayoutBinding.stageFlags = getVkShaderStageFlagBits(binding.stageFlags);
				descriptorSetLayoutBinding.pImmutableSamplers = nullptr;

				descriptorSetLayoutBindings.emplace_back(descriptorSetLayoutBinding);

				VkDescriptorPoolSize descriptorPoolSize{};
				descriptorPoolSize.type = descriptorType;

				if (
					binding.descriptorType == DescriptorType::COMBINED_IMAGE_SAMPLER ||
					binding.descriptorType == DescriptorType::STORAGE_IMAGE
				) {
					ENGINE_ASSERT(binding.imageViews.size() == binding.samplers.size(), "Mismatch between image view count and sampler count.");

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

			ENGINE_ASSERT(result == VK_SUCCESS, "Failed to create Vulkan descriptor set layout (code: []).", result);

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

			ENGINE_ASSERT(result == VK_SUCCESS, "Failed to create Vulkan descriptor pool (code: []).", result);

			VkDescriptorSetAllocateInfo descriptorSetAllocateInfo{};
			descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			descriptorSetAllocateInfo.pNext = nullptr;
			descriptorSetAllocateInfo.descriptorPool = descriptorPool;
			descriptorSetAllocateInfo.descriptorSetCount = static_cast<uint32>(descriptorSetLayouts.size());
			descriptorSetAllocateInfo.pSetLayouts = descriptorSetLayouts.data();

			descriptorSets.resize(descriptorSetLayouts.size());

			result = vkAllocateDescriptorSets(device, &descriptorSetAllocateInfo, descriptorSets.data());

			ENGINE_ASSERT(result == VK_SUCCESS, "Failed to allocate Vulkan descriptor sets (code: []).", result);

			chain<VkDescriptorBufferInfo> descriptorBufferInfos;
			chain<VkDescriptorImageInfo> descriptorImageInfos;

			for (const DescriptorSetLayout& layout : descriptor.layouts) {
				for (const DescriptorSetLayoutBinding& binding : layout.bindings) {
					for (const ref<Buffer>& buffer : binding.buffers) {
						const VulkanBuffer* vkBuffer = static_cast<const VulkanBuffer*>(buffer.get());

						VkDescriptorBufferInfo descriptorBufferInfo{};
						descriptorBufferInfo.buffer = vkBuffer->object;
						descriptorBufferInfo.offset = 0;
						descriptorBufferInfo.range = vkBuffer->size;

						descriptorBufferInfos.emplace_back(descriptorBufferInfo);
					}

					for (uint32 imageViewIndex = 0; imageViewIndex < binding.imageViews.size(); imageViewIndex += 1) {
						const ref<VulkanSampler> sampler = std::static_pointer_cast<VulkanSampler>(binding.samplers[imageViewIndex]);
						const ref<VulkanImageView> imageView = std::static_pointer_cast<VulkanImageView>(binding.imageViews[imageViewIndex]);

						VkDescriptorImageInfo descriptorImageInfo{};
						descriptorImageInfo.sampler = sampler->object;
						descriptorImageInfo.imageView = imageView->object;

						// TODO: Specify layout of current image.
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

			chain<VkWriteDescriptorSet> writeDescriptorSets;

			uint32 bufferWriteDescriptorSetIndex = 0;
			uint32 imageWriteDescriptorSetIndex = 0;
			uint32 storageImageWriteDescriptorSetIndex = 0;

			for (uint32 layoutIndex = 0; layoutIndex < descriptor.layouts.size(); layoutIndex += 1) {
				const DescriptorSetLayout& layout = descriptor.layouts[layoutIndex];

				for (uint32 bindingIndex = 0; bindingIndex < layout.bindings.size(); bindingIndex += 1) {
					const DescriptorSetLayoutBinding& binding = layout.bindings[bindingIndex];

					// 1 write descriptor set per binding.

					for (uint32 bufferIndex = 0; bufferIndex < binding.buffers.size(); bufferIndex += 1) {
						VkWriteDescriptorSet writeDescriptorSet{};
						writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
						writeDescriptorSet.pNext = nullptr;
						writeDescriptorSet.dstSet = descriptorSets[layoutIndex];
						writeDescriptorSet.dstBinding = bindingIndex;
						writeDescriptorSet.dstArrayElement = 0;
						writeDescriptorSet.descriptorCount = 1;
						writeDescriptorSet.descriptorType = getVkDescriptorType(binding.descriptorType);
						writeDescriptorSet.pImageInfo = nullptr;
						writeDescriptorSet.pBufferInfo = &descriptorBufferInfos[bufferWriteDescriptorSetIndex];
						writeDescriptorSet.pTexelBufferView = nullptr;

						writeDescriptorSets.emplace_back(writeDescriptorSet);

						bufferWriteDescriptorSetIndex += 1;
					}

					if (binding.imageViews.size() > 0) {
						// Treat as an image array.
						VkWriteDescriptorSet writeDescriptorSet{};
						writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
						writeDescriptorSet.pNext = nullptr;
						writeDescriptorSet.dstSet = descriptorSets[layoutIndex];
						writeDescriptorSet.dstBinding = bindingIndex;
						writeDescriptorSet.dstArrayElement = 0;
						writeDescriptorSet.descriptorCount = 1;
						writeDescriptorSet.descriptorType = getVkDescriptorType(binding.descriptorType);
						writeDescriptorSet.pImageInfo = &descriptorImageInfos[imageWriteDescriptorSetIndex];
						writeDescriptorSet.pBufferInfo = nullptr;
						writeDescriptorSet.pTexelBufferView = nullptr;

						writeDescriptorSets.emplace_back(writeDescriptorSet);

						imageWriteDescriptorSetIndex += 1;
					}
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

		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to create Vulkan pipeline layout (code: []).", result);
	}

	VulkanPipelineLayout::~VulkanPipelineLayout() {
		PROFILE_SCOPE(FUNC_SIG);

		if (descriptorSetLayouts.size() > 0) {
			// NOTE: Only if the pool has been created with the VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT flag.
			/*const VkResult result = vkFreeDescriptorSets(device, descriptorPool, static_cast<uint32>(descriptorSets.size()), descriptorSets.data());

			ENGINE_ASSERT(result == VK_SUCCESS, "Failed to free Vulkan descriptor sets (code: []).", result);*/

			vkDestroyDescriptorPool(device, descriptorPool, nullptr);

			for (const VkDescriptorSetLayout descriptorSetLayout : descriptorSetLayouts) {
				vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);
			}
		}

		vkDestroyPipelineLayout(device, object, nullptr);
	}
}