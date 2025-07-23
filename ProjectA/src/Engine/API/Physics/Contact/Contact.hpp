#pragma once

namespace ProjectA {
	struct Contact2D {
		vec2<float32> position;

		// Distance to the normal of the reference edge.
		float32 separation;

		// A's position to the contact.
		vec2<float32> rA;

		// B's position to the contact.
		vec2<float32> rB;

		float32 massNormal;
		float32 massTangent;
		float32 bias;

		float32 accumulatedNormalImpulse = 0.0f;
		float32 accumulatedTangentImpulse = 0.0f;
	};
}