#pragma once

#include "Engine/API/Physics/Contact/Contact.hpp"

namespace ProjectA {
	struct ContactManifold2D {
		vec2<float32> normal;
		vec2<float32> tangent;
		uint8 contactCount;
		Contact2D contacts[2];
	};
}