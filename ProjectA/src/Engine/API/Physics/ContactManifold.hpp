#pragma once

#include "Engine/API/Physics/ClosestFeature2D.hpp"

namespace ProjectA {
	struct Contact2D {
		vec2<float32> position;
		// vec2<float32> normal;

		// A's position to the contact.
		vec2<float32> rA;

		// B's position to the contact.
		vec2<float32> rB;

		float32 massNormal;
		float32 massTangent;
		float32 bias;

		float32 accumulatedNormalImpulse;
		float32 accumulatedTangentImpulse;

		// ClosestFeature2D closestFeature;
	};

	struct ContactManifold2D {
		vec2<float32> contactNormal;
		float32 penetration;

		// TODO: Use fewer bits.
		uint8 contactCount;

		Contact2D contacts[2];

		// TODO: Differentiate static friction from dynamic friction.
		float32 combinedFriction;
	};
}