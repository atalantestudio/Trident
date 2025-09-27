#pragma once

namespace ProjectA {
	// See https://stackoverflow.com/a/77070649/17136841.
	enum class DescriptorType : uint8 {
		COMBINED_IMAGE_SAMPLER = 1,
		STORAGE_IMAGE = 3,
		UNIFORM_BUFFER = 6,
		STORAGE_BUFFER = 7,
		INPUT_ATTACHMENT = 10,

		// TODO: Add SAMPLER.
		// TODO: Add SAMPLED_IMAGE.

		__MAX_ENUM,
	};
}