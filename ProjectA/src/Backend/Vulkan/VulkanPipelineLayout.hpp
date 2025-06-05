#pragma once

#include "Engine/API/ObjectWrapper.hpp"
#include "Engine/API/Rendering/Descriptor/PipelineLayoutDescriptor.hpp"
#include "Engine/API/Rendering/PipelineLayout.hpp"

namespace ProjectA {
	class VulkanPipelineLayout : public PipelineLayout, public ObjectWrapper<VkPipelineLayout> {
		public:
			VulkanPipelineLayout(VkDevice device, const PipelineLayoutDescriptor& descriptor);

			~VulkanPipelineLayout();

		public:
			std::vector<VkDescriptorSet> descriptorSets;

		private:
			const VkDevice device;
			std::vector<VkDescriptorSetLayout> descriptorSetLayouts;
			VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
	};
}