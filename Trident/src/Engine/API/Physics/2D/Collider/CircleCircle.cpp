// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Physics/2D/Collider/Collider.hpp"

namespace Trident {
	bool CircleCircle2D(const Hull2D& shapeA, const Hull2D& shapeB, const RigidBody2D& rigidBodyA, const RigidBody2D& rigidBodyB, ContactManifold2D* manifold) {
		const vec2<float32> AB = rigidBodyB.position - rigidBodyA.position;
		const float32 lengthAB = lengthSquared(AB);

		if (abs(lengthAB) < 0.001f) {
			// Degenerate case: The circles are coincident.
			return false;
		}

		// The scaling factor is assumed to be the same in both axes.
		const float32 scaleA = rigidBodyA.scale[0];
		const float32 scaleB = rigidBodyB.scale[0];

		const float32 d = lengthAB - pow(shapeA.radius * scaleA + shapeB.radius * scaleB, 2.0f);

		if (d > 0.0f) {
			return false;
		}

		const vec2<float32> sA = rigidBodyA.position + shapeA.support(AB) * scaleA;
		const vec2<float32> sB = rigidBodyB.position + shapeB.support(-AB) * scaleB;

		manifold->normal = normalized(AB);
		manifold->contactCount = 1;
		manifold->contacts[0].position = (sA + sB) * 0.5f;
		manifold->contacts[0].separation = d;

		return true;
	}
}