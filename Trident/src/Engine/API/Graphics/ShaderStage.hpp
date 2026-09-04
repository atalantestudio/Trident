// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	enum class ShaderStage : uint8 {
		VERTEX = BIT(0),
		FRAGMENT = BIT(1),
		COMPUTE = BIT(2),

		// EXT_hardware_ray_tracing
		RAY_GENERATION = BIT(3),
		RAY_CLOSEST_HIT = BIT(4),
		RAY_MISS = BIT(5),
		RAY_ANY_HIT = BIT(6),
		RAY_INTERSECTION = BIT(7),
	};

	AS_FLAG(ShaderStage);
}