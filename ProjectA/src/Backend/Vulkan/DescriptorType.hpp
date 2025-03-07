#pragma once

#include <vulkan/vulkan.h>

#include "Engine/Renderer/DescriptorType.hpp"

namespace ProjectA {
	static VkDescriptorType getVkDescriptorType(const DescriptorType descriptorType) {
		static constexpr VkDescriptorType vkDescriptorTypes[] = {
			VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
			VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
		};

		return vkDescriptorTypes[static_cast<uint32>(descriptorType)];
	}
}