#pragma once

#include "Engine/API/ObjectWrapper.hpp"
#include "Engine/API/Rendering/DescriptorSetLayout.hpp"

namespace ProjectA {
	class VulkanDescriptorSetLayout final : public DescriptorSetLayout, public ObjectWrapper<VkDescriptorSetLayout> {
		public:
			explicit VulkanDescriptorSetLayout(VkDevice device, const sequence<DescriptorSetLayoutBinding>& descriptorSetLayoutBindings);

			~VulkanDescriptorSetLayout() final override;

		private:
			const VkDevice device;
	};
}