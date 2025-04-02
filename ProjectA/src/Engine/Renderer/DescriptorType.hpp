#pragma once

#include "pch.hpp"

namespace ProjectA {
	enum class DescriptorType : uint8 {
		STORAGE_BUFFER = 0,
		UNIFORM_BUFFER = 1,
		COMBINED_IMAGE_SAMPLER = 2,
		STORAGE_IMAGE = 3,
	};
}