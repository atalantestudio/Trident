#pragma once

#include "Engine/API/Rendering/DescriptorType.hpp"
#include "Engine/API/Rendering/ShaderStage.hpp"

namespace Trident {
	struct DescriptorSetLayoutBinding {
		DescriptorType descriptorType;
		uint32 descriptorCount;
		ShaderStage shaderStages;
	};

	class DescriptorSetLayout {
		public:
			explicit DescriptorSetLayout(const sequence<DescriptorSetLayoutBinding>& descriptorSetLayoutBindings) :
				bindings(descriptorSetLayoutBindings)
			{}

			virtual ~DescriptorSetLayout() = default;

		public:
			sequence<DescriptorSetLayoutBinding> bindings;
	};
}