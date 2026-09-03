// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	// See https://stackoverflow.com/a/77070649/17136841.
	enum class DescriptorType : uint8 {
		COMBINED_IMAGE_SAMPLER,
		READ_ONLY_STORAGE_IMAGE,
		READ_WRITE_STORAGE_IMAGE,
		INPUT_ATTACHMENT,
		UNIFORM_BUFFER,
		READ_ONLY_STORAGE_BUFFER,
		READ_WRITE_STORAGE_BUFFER,

		// EXT_hardware_ray_tracing
		ACCELERATION_STRUCTURE,

		count,
	};

	inline bool isBufferDescriptorType(DescriptorType descriptorType) {
		static constexpr DescriptorType start = DescriptorType::UNIFORM_BUFFER;
		static constexpr DescriptorType end = DescriptorType::READ_WRITE_STORAGE_BUFFER;

		return descriptorType >= start && descriptorType <= end;
	}

	inline bool isImageDescriptorType(DescriptorType descriptorType) {
		static constexpr DescriptorType start = DescriptorType::COMBINED_IMAGE_SAMPLER;
		static constexpr DescriptorType end = DescriptorType::INPUT_ATTACHMENT;

		return descriptorType >= start && descriptorType <= end;
	}

	inline bool isSamplerDescriptorType(DescriptorType descriptorType) {
		static constexpr DescriptorType start = DescriptorType::COMBINED_IMAGE_SAMPLER;
		static constexpr DescriptorType end = DescriptorType::COMBINED_IMAGE_SAMPLER;

		return descriptorType >= start && descriptorType <= end;
	}
}