#pragma once

#include "Engine/API/ObjectWrapper.hpp"
#include "Engine/API/Rendering/DescriptorSet.hpp"

namespace ProjectA {
	class VulkanDescriptorSet final : public DescriptorSet, public ObjectWrapper<VkDescriptorSet> {
		public:
			struct DescriptorSetBinding {
				DescriptorType descriptorType;
			};

		public:
			explicit VulkanDescriptorSet(VkDescriptorSet descriptorSet) :
				ObjectWrapper(descriptorSet)
			{}

		public:
			sequence<DescriptorSetBinding> bindings;
	};
}