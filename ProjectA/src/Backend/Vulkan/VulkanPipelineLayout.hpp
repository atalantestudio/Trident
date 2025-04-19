#pragma once

#include "Engine/API/ObjectWrapper.hpp"
#include "Engine/Renderer/PipelineLayout.hpp"

namespace ProjectA {
	class VulkanPipelineLayout : public PipelineLayout, public ObjectWrapper<VkPipelineLayout> {
		public:
			VulkanPipelineLayout(VkDevice device, const PipelineLayoutDescriptor& descriptor);

			~VulkanPipelineLayout();

		public:
			chain<VkDescriptorSet> descriptorSets;

		private:
			const VkDevice device;
			chain<VkDescriptorSetLayout> descriptorSetLayouts;
			VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
	};
}