#pragma once

#include "Engine/API/Rendering/DescriptorType.hpp"

namespace ProjectA {
	static VkDescriptorType getVkDescriptorType(DescriptorType descriptorType) {
		return static_cast<VkDescriptorType>(descriptorType);
	}
}