// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/DescriptorSetLayout.hpp"
#include "Engine/API/Graphics/Device.hpp"

namespace Trident {
	class VK::DescriptorSetLayout final : public Trident::DescriptorSetLayout {
		public:
			explicit DescriptorSetLayout(const Trident::Device* device, view<DescriptorSetLayoutBinding> bindings);

		public:
			VkDescriptorSetLayout descriptorSetLayout;
	};

	namespace VK {
		ref<Trident::DescriptorSetLayout> createDescriptorSetLayout(const Trident::Device* device, view<DescriptorSetLayoutBinding> bindings);

		void destroyDescriptorSetLayout(const Trident::Device* device, const Trident::DescriptorSetLayout* descriptorSetLayout);
	}
}