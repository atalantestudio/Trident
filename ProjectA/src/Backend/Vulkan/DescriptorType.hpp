#pragma once

namespace ProjectA {
	static VkDescriptorType getVkDescriptorType(DescriptorType descriptorType) {
		return static_cast<VkDescriptorType>(descriptorType);
	}
}