// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Physics/Collider/Collider2D.hpp"

namespace Trident {
	static bool CircleCircleCollider(
		const RigidBody2DComponent& A,
		const RigidBody2DComponent& B,
		const Hull2D& hullA,
		const Hull2D& hullB,
		ContactManifold2D* contactManifold
	) {
		const vec2<float32> AB = B.position - A.position;
		const float32 lengthAB = lengthSquared(AB);

		if (abs(lengthAB) < 0.001f) {
			// Degenerate case: The circles are coincident.
			return false;
		}

		// The scaling factor is assumed to be the same in both axes.
		const float32 scaleA = A.scale[0];
		const float32 scaleB = B.scale[0];

		const float32 d = lengthAB - pow(hullA.getRadius() * scaleA + hullB.getRadius() * scaleB, 2.0f);

		if (d > 0.0f) {
			return false;
		}

		const vec2<float32> sA = A.position + hullA.support(AB) * scaleA;
		const vec2<float32> sB = B.position + hullB.support(-AB) * scaleB;

		contactManifold->normal = normalized(AB);
		contactManifold->contactCount = 1;
		contactManifold->contacts[0].position = (sA + sB) * 0.5f;
		contactManifold->contacts[0].separation = d;

		return true;
	}
}