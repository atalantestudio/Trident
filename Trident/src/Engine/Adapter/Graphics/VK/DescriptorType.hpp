// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/DescriptorType.hpp"

namespace Trident::VK {
	inline VkDescriptorType getDescriptorType(DescriptorType descriptorType) {
		return static_cast<VkDescriptorType>(descriptorType);
	}
}