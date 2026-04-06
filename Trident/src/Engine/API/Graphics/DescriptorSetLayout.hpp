// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/Base.hpp"
#include "Engine/API/Graphics/DescriptorType.hpp"
#include "Engine/API/Graphics/ShaderStage.hpp"

namespace Trident {
	struct DescriptorSetLayoutBinding {
		DescriptorType descriptorType;
		uint32 descriptorCount;
		ShaderStage shaderStages;
	};

	class DescriptorSetLayout {
		public:
			explicit DescriptorSetLayout(const Device* device, const sequence<DescriptorSetLayoutBinding>& bindings);

			~DescriptorSetLayout();

		public:
			const Device* device;
			sequence<DescriptorSetLayoutBinding> bindings;
	};
}