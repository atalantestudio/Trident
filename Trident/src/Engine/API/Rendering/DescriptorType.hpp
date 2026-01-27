// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	// See https://stackoverflow.com/a/77070649/17136841.
	enum class DescriptorType : uint8 {
		COMBINED_IMAGE_SAMPLER = 1,
		STORAGE_IMAGE = 3,
		UNIFORM_BUFFER = 6,
		STORAGE_BUFFER = 7,
		INPUT_ATTACHMENT = 10,

		// TODO: Add SAMPLER.
		// TODO: Add SAMPLED_IMAGE.

		count,
	};

	static bool isBufferDescriptorType(DescriptorType descriptorType) {
		DEBUG_ASSERT(descriptorType < DescriptorType::count);

		switch (descriptorType) {
			case DescriptorType::COMBINED_IMAGE_SAMPLER:
			case DescriptorType::STORAGE_IMAGE:
			case DescriptorType::INPUT_ATTACHMENT:
				return false;
			case DescriptorType::UNIFORM_BUFFER:
			case DescriptorType::STORAGE_BUFFER:
				return true;
		}

		return false;
	}

	static bool isImageDescriptorType(DescriptorType descriptorType) {
		DEBUG_ASSERT(descriptorType < DescriptorType::count);

		switch (descriptorType) {
			case DescriptorType::COMBINED_IMAGE_SAMPLER:
			case DescriptorType::STORAGE_IMAGE:
			case DescriptorType::INPUT_ATTACHMENT:
				return true;
			case DescriptorType::UNIFORM_BUFFER:
			case DescriptorType::STORAGE_BUFFER:
				return false;
		}

		return false;
	}
}