#pragma once

#include <vulkan/vulkan.h>

#include "Engine/Renderer/DescriptorType.hpp"

namespace ProjectA {
	static VkDescriptorType getVkDescriptorType(DescriptorType descriptorType) {
		static constexpr VkDescriptorType vkDescriptorTypes[] = {
			VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
			VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
		};

		return vkDescriptorTypes[static_cast<uint32>(descriptorType)];
	}
}