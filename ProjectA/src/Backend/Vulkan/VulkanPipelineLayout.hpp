#pragma once

#include <vulkan/vulkan.h>

#include "Engine/ObjectWrapper.hpp"
#include "Engine/Renderer/PipelineLayout.hpp"

namespace ProjectA {
	class VulkanPipelineLayout : public PipelineLayout, public ObjectWrapper<VkPipelineLayout> {
		public:
			VulkanPipelineLayout(const VkDevice device, const PipelineLayoutDescriptor& descriptor);

			~VulkanPipelineLayout();

		public:
			std::vector<VkDescriptorSet> descriptorSets;

		private:
			const VkDevice device;
			std::vector<VkDescriptorSetLayout> descriptorSetLayouts;
			VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
	};
}