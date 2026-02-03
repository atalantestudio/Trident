// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/GraphicsAdapter.hpp"
#include "Engine/API/Rendering/DescriptorType.hpp"

namespace Trident::VK {
	inline VkDescriptorType getDescriptorType(DescriptorType descriptorType) {
		switch (descriptorType) {
			case DescriptorType::COMBINED_IMAGE_SAMPLER: return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			case DescriptorType::READ_ONLY_STORAGE_IMAGE: return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
			case DescriptorType::STORAGE_IMAGE: return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
			case DescriptorType::UNIFORM_BUFFER: return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			case DescriptorType::READ_ONLY_STORAGE_BUFFER: return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			case DescriptorType::READ_WRITE_STORAGE_BUFFER: return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			case DescriptorType::INPUT_ATTACHMENT: return VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
			default: {
				ADAPTER_ASSERT(false, "Unknown descriptor type.");

				return VK_DESCRIPTOR_TYPE_MAX_ENUM;
			}
		}
	}
}