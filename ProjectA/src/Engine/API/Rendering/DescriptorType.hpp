#pragma once

namespace ProjectA {
	// TODO: Add SAMPLER.
	// TODO: Add SAMPLED_IMAGE.
	// See https://stackoverflow.com/a/77070649/17136841.
	enum class DescriptorType : uint8 {
		STORAGE_BUFFER = 0,
		UNIFORM_BUFFER = 1,
		COMBINED_IMAGE_SAMPLER = 2,
		STORAGE_IMAGE = 3,
	};
}