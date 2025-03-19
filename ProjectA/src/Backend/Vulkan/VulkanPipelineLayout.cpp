#include "pch.hpp"

#include "Backend/Vulkan/DescriptorType.hpp"
#include "Backend/Vulkan/ShaderStage.hpp"
#include "Backend/Vulkan/VulkanBuffer.hpp"
#include "Backend/Vulkan/VulkanPipelineLayout.hpp"
#include "Backend/Vulkan/VulkanSampler.hpp"
#include "Backend/Vulkan/VulkanTextureView.hpp"

// Shaders have descriptor sets.
// 1 descriptor set can have multiple bindings.
// Each binding has its own type, from UNIFORM to COMBINED_IMAGE_SAMPLER.
// In this case the descriptor set 0 will have a UNIFORM at the binding 0.
//
// There is 1 descriptor pool per descriptor set.
// The pool must match the type and sizes of the bindings in the set.
namespace ProjectA {
	VulkanPipelineLayout::VulkanPipelineLayout(const VkDevice device, const PipelineLayoutDescriptor& descriptor) :
		device(device)
	{
		PROFILE_SCOPE(FUNC_SIG);

		ENGINE_TRACE("[VulkanPipelineLayout]");

		std::vector<VkDescriptorPoolSize> descriptorPoolSizes;

		for (const DescriptorSetLayout& layout : descriptor.layouts) {
			std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings;

			for (uint32 bindingIndex = 0; bindingIndex < layout.bindings.size(); bindingIndex++) {
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

				if (binding.descriptorType == DescriptorType::COMBINED_IMAGE_SAMPLER) {
					ENGINE_ASSERT(binding.textureViews.size() == binding.samplers.size(), "Mismatch between texture view count and sampler count.");

					descriptorPoolSize.descriptorCount = static_cast<uint32>(binding.textureViews.size());
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

			if (vkCreateDescriptorSetLayout(device, &descriptorSetLayoutCreateInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
				throw std::runtime_error("Failed to create descriptor set layout.");
			}

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

			if (vkCreateDescriptorPool(device, &descriptorPoolCreateInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
				throw std::runtime_error("Failed to create descriptor pool.");
			}

			VkDescriptorSetAllocateInfo descriptorSetAllocateInfo{};
			descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			descriptorSetAllocateInfo.pNext = nullptr;
			descriptorSetAllocateInfo.descriptorPool = descriptorPool;
			descriptorSetAllocateInfo.descriptorSetCount = static_cast<uint32>(descriptorSetLayouts.size());
			descriptorSetAllocateInfo.pSetLayouts = descriptorSetLayouts.data();

			descriptorSets.resize(descriptorSetLayouts.size());

			if (vkAllocateDescriptorSets(device, &descriptorSetAllocateInfo, descriptorSets.data()) != VK_SUCCESS) {
				throw std::runtime_error("Failed to allocate descriptor sets.");
			}

			std::vector<VkWriteDescriptorSet> writeDescriptorSets;
			std::vector<VkDescriptorBufferInfo> descriptorBufferInfos;
			std::vector<VkDescriptorImageInfo> descriptorImageInfos;

			for (uint32 setLayoutIndex = 0; setLayoutIndex < descriptor.layouts.size(); setLayoutIndex++) {
				const DescriptorSetLayout& setLayout = descriptor.layouts[setLayoutIndex];

				for (uint32 bindingIndex = 0; bindingIndex < setLayout.bindings.size(); bindingIndex++) {
					const DescriptorSetLayoutBinding& binding = setLayout.bindings[bindingIndex];

					for (uint32 bufferIndex = 0; bufferIndex < binding.buffers.size(); bufferIndex++) {
						const VulkanBuffer* buffer = static_cast<const VulkanBuffer*>(binding.buffers[bufferIndex].get());

						VkDescriptorBufferInfo descriptorBufferInfo{};
						descriptorBufferInfo.buffer = buffer->object;
						descriptorBufferInfo.offset = 0;
						descriptorBufferInfo.range = buffer->size;

						descriptorBufferInfos.emplace_back(descriptorBufferInfo);
					}

					for (uint32 textureViewIndex = 0; textureViewIndex < binding.textureViews.size(); textureViewIndex++) {
						const VulkanSampler* sampler = static_cast<const VulkanSampler*>(binding.samplers[textureViewIndex].get());
						const VulkanTextureView* textureView = static_cast<const VulkanTextureView*>(binding.textureViews[textureViewIndex].get());

						VkDescriptorImageInfo descriptorImageInfo{};
						descriptorImageInfo.sampler = sampler->object;
						descriptorImageInfo.imageView = textureView->object;
						descriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

						descriptorImageInfos.emplace_back(descriptorImageInfo);
					}
				}
			}

			uint32 writeDescriptorSetIndex = 0;

			for (uint32 setLayoutIndex = 0; setLayoutIndex < descriptor.layouts.size(); setLayoutIndex++) {
				const DescriptorSetLayout& setLayout = descriptor.layouts[setLayoutIndex];

				for (uint32 bindingIndex = 0; bindingIndex < setLayout.bindings.size(); bindingIndex++) {
					const DescriptorSetLayoutBinding& binding = setLayout.bindings[bindingIndex];

					for (uint32 bufferIndex = 0; bufferIndex < binding.buffers.size(); bufferIndex++) {
						// 1 write descriptor set per descriptor set and per binding index.
						VkWriteDescriptorSet writeDescriptorSet{};
						writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
						writeDescriptorSet.pNext = nullptr;
						writeDescriptorSet.dstSet = descriptorSets[setLayoutIndex];
						writeDescriptorSet.dstBinding = bindingIndex;
						writeDescriptorSet.dstArrayElement = 0;
						writeDescriptorSet.descriptorCount = 1;
						writeDescriptorSet.descriptorType = getVkDescriptorType(binding.descriptorType);
						writeDescriptorSet.pImageInfo = nullptr;
						writeDescriptorSet.pBufferInfo = &descriptorBufferInfos[writeDescriptorSetIndex];
						writeDescriptorSet.pTexelBufferView = nullptr;

						writeDescriptorSets.emplace_back(writeDescriptorSet);

						writeDescriptorSetIndex++;
					}

					for (uint32 textureViewIndex = 0; textureViewIndex < binding.textureViews.size(); textureViewIndex++) {
						// 1 write descriptor set per descriptor set and per binding index.
						VkWriteDescriptorSet writeDescriptorSet{};
						writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
						writeDescriptorSet.pNext = nullptr;
						writeDescriptorSet.dstSet = descriptorSets[setLayoutIndex];
						writeDescriptorSet.dstBinding = bindingIndex;
						writeDescriptorSet.dstArrayElement = 0;
						writeDescriptorSet.descriptorCount = 1;
						writeDescriptorSet.descriptorType = getVkDescriptorType(binding.descriptorType);
						writeDescriptorSet.pImageInfo = &descriptorImageInfos[writeDescriptorSetIndex];
						writeDescriptorSet.pBufferInfo = nullptr;
						writeDescriptorSet.pTexelBufferView = nullptr;

						writeDescriptorSets.emplace_back(writeDescriptorSet);

						writeDescriptorSetIndex++;
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

		if (vkCreatePipelineLayout(device, &pipelineLayoutCreateInfo, nullptr, &object) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create pipeline layout.");
		}
	}

	VulkanPipelineLayout::~VulkanPipelineLayout() {
		PROFILE_SCOPE(FUNC_SIG);

		ENGINE_TRACE("[~VulkanPipelineLayout]");

		if (descriptorSetLayouts.size() > 0) {
			// TODO: Only OK if the pool has been created with the VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT flag.
			//vkFreeDescriptorSets(device, descriptorPool, static_cast<uint32>(descriptorSets.size()), descriptorSets.data());

			vkDestroyDescriptorPool(device, descriptorPool, nullptr);

			for (const VkDescriptorSetLayout descriptorSetLayout : descriptorSetLayouts) {
				vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);
			}
		}

		vkDestroyPipelineLayout(device, object, nullptr);
	}
}