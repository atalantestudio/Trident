// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/DescriptorSet.hpp"
#include "Engine/API/Graphics/DescriptorType.hpp"

namespace Trident {
	class VK::DescriptorSet final : public Trident::DescriptorSet {
		public:
			explicit DescriptorSet(VkDescriptorSet descriptorSet) :
				descriptorSet(descriptorSet)
			{}

		public:
			VkDescriptorSet descriptorSet;
			sequence<DescriptorSetLayoutBinding> bindings;
	};
}