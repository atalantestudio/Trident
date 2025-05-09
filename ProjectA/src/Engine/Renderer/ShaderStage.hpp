#pragma once

// TODO: Remove PCH include.
#include "pch.hpp"

namespace ProjectA {
	enum class ShaderStage : uint8 {
		VERTEX = BIT(0),
		FRAGMENT = BIT(1),
		COMPUTE = BIT(2),
	};

	// TODO: Does this need to be AS_FLAG since we now have PipelineStage?
	AS_FLAG(ShaderStage);
}