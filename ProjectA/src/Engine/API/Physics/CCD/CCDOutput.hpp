#pragma once

#include "Engine/API/Physics/CCD/CCDOutputState.hpp"

namespace ProjectA {
	struct CCDOutput {
		CCDOutputState state;

		// Time in between 2 frames at which point the objects collide.
		float32 fraction;

		vec2<float32> closestA;
		vec2<float32> closestB;
	};
}