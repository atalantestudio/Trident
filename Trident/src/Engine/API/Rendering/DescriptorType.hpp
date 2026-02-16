// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	// See https://stackoverflow.com/a/77070649/17136841.
	enum class DescriptorType : uint8 {
		COMBINED_IMAGE_SAMPLER,
		READ_ONLY_STORAGE_IMAGE,

		// TODO: Separate from READ_ONLY_STORAGE_IMAGE.
		STORAGE_IMAGE,

		INPUT_ATTACHMENT,
		UNIFORM_BUFFER,
		READ_ONLY_STORAGE_BUFFER,
		READ_WRITE_STORAGE_BUFFER,

		count,
	};

	static bool isBufferDescriptorType(DescriptorType descriptorType) {
		static constexpr DescriptorType firstBufferDescriptorType = DescriptorType::UNIFORM_BUFFER;

		DEBUG_ASSERT(descriptorType < DescriptorType::count);

		return descriptorType >= firstBufferDescriptorType;
	}

	static bool isImageDescriptorType(DescriptorType descriptorType) {
		static constexpr DescriptorType lastImageDescriptorType = DescriptorType::INPUT_ATTACHMENT;

		DEBUG_ASSERT(descriptorType < DescriptorType::count);

		return descriptorType <= lastImageDescriptorType;
	}

	static bool isSamplerDescriptorType(DescriptorType descriptorType) {
		DEBUG_ASSERT(descriptorType < DescriptorType::count);

		return descriptorType == DescriptorType::COMBINED_IMAGE_SAMPLER;
	}
}