// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/DescriptorSet.hpp"
#include "Engine/API/Rendering/DescriptorType.hpp"

namespace ProjectA {
	class VK::DescriptorSet final : public ProjectA::DescriptorSet {
		public:
			struct DescriptorSetBinding {
				DescriptorType descriptorType;
			};

		public:
			explicit DescriptorSet(VkDescriptorSet descriptorSet) :
				descriptorSet(descriptorSet)
			{}

		public:
			VkDescriptorSet descriptorSet;
			sequence<DescriptorSetBinding> bindings;
	};
}