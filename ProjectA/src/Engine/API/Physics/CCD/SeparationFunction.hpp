#pragma once

#include "Engine/API/Physics/CCD/SeparationType.hpp"

namespace ProjectA {
	struct SeparationFunction {
		SeparationType separationType;

		// The separating axis is only used as a direction to find support points,
		// and does not need to be normalized.
		vec2<float32> separatingAxis;

		// The average of the separation points.
		vec2<float32> localPoint;
	};
}