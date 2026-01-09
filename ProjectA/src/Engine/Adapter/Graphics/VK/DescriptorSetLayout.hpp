// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/VK/Device.hpp"
#include "Engine/API/Rendering/DescriptorSetLayout.hpp"

namespace ProjectA {
	class VK::DescriptorSetLayout final : public ProjectA::DescriptorSetLayout {
		public:
			explicit DescriptorSetLayout(const sequence<DescriptorSetLayoutBinding>& descriptorSetLayoutBindings, const VK::Device& device);

			~DescriptorSetLayout() final override;

		public:
			VkDescriptorSetLayout descriptorSetLayout;

		private:
			const VK::Device& device;
	};
}