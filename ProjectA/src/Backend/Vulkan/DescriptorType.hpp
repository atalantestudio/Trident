#pragma once

#include "Engine/Renderer/DescriptorType.hpp"

namespace ProjectA {
	static VkDescriptorType getVkDescriptorType(DescriptorType descriptorType) {
		static constexpr VkDescriptorType vkDescriptorTypes[] = {
			VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
			VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
			VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
		};

		return vkDescriptorTypes[static_cast<uint8>(descriptorType)];
	}
}