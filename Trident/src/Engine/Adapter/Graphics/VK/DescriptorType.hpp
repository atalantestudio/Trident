// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/GraphicsAdapter.hpp"
#include "Engine/API/Graphics/DescriptorType.hpp"

namespace Trident::VK {
	inline VkDescriptorType getDescriptorType(DescriptorType descriptorType) {
		static constexpr VkDescriptorType DESCRIPTOR_TYPES[] = {
			VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
			VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
			VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
			VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,
			VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
			VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
		};

		ADAPTER_ASSERT(descriptorType < DescriptorType::count, "Unknown descriptor type.");

		return DESCRIPTOR_TYPES[static_cast<uint32>(descriptorType)];
	}
}