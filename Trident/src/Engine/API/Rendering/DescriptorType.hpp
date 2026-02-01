// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	// See https://stackoverflow.com/a/77070649/17136841.
	enum class DescriptorType : uint8 {
		COMBINED_IMAGE_SAMPLER,
		STORAGE_IMAGE,
		INPUT_ATTACHMENT,
		UNIFORM_BUFFER,

		// NOTE: (DirectX 12) Maps to a shader resource view.
		READ_ONLY_STORAGE_BUFFER,

		// NOTE: (DirectX 12) Maps to a unordered access view.
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
}